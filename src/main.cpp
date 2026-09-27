#include <opencv2/opencv.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <limits>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "Capture.h"
#include "Detector.h"
#include "Encoder.h"
#include "EventLog.h"
#include "FaceDatabase.h"
#include "Matcher.h"
#include "Persistence.h"

struct FaceAnnotation
{
    cv::Rect rect;
    std::array<cv::Point, 68> landmarks{};
    bool hasLandmarks = false;
    std::string label;
};

struct SharedState
{
    cv::Mat latestFrame;
    std::uint64_t frameVersion = 0;
    std::mutex frameMutex;
    std::condition_variable frameCondition;
    bool recognitionPaused = false;
    bool recognitionWorkerPaused = false;

    std::vector<FaceAnnotation> annotations;
    std::array<float, Encoder::kEmbeddingSize> latestEmbedding{};
    bool hasLatestEmbedding = false;
    std::mutex resultMutex;
    std::mutex databaseMutex;
    std::atomic<bool> running{true};
};

void captureLoop(Capture &capture, SharedState &state)
{
    while (state.running)
    {
        cv::Mat frame;
        if (!capture.readFrame(frame))
        {
            std::cerr << "Failed to read frame from camera\n";
            state.running = false;
            state.frameCondition.notify_all();
            break;
        }

        {
            std::lock_guard<std::mutex> lock(state.frameMutex);
            state.latestFrame = std::move(frame);
            ++state.frameVersion;
        }
        state.frameCondition.notify_one();
    }
}

void recognitionLoop(SharedState &state, Detector &detector, Encoder &encoder,
                     FaceDatabase &database, EventLog &eventLog)
{
    std::uint64_t processedVersion = 0;
    int lastReportedRecordId = -2;
    while (state.running)
    {
        cv::Mat frame;
        {
            std::unique_lock<std::mutex> lock(state.frameMutex);
            state.frameCondition.wait(lock, [&state, processedVersion]
                                      { return !state.running || state.recognitionPaused ||
                                               state.frameVersion > processedVersion; });
            if (!state.running)
            {
                break;
            }
            if (state.recognitionPaused)
            {
                state.recognitionWorkerPaused = true;
                state.frameCondition.notify_all();
                state.frameCondition.wait(lock, [&state]
                                          { return !state.running || !state.recognitionPaused; });
                state.recognitionWorkerPaused = false;
                state.frameCondition.notify_all();
                continue;
            }
            frame = state.latestFrame.clone();
            processedVersion = state.frameVersion;
        }

        std::vector<FaceAnnotation> annotations;
        std::array<float, Encoder::kEmbeddingSize> latestEmbedding{};
        bool hasLatestEmbedding = false;
        int firstFaceRecordId = -2;
        std::string firstFaceName;

        for (const cv::Rect &face : detector.detect(frame))
        {
            FaceAnnotation annotation;
            annotation.rect = face;

            if (encoder.encode(frame, face, latestEmbedding.data(), annotation.landmarks.data()))
            {
                annotation.hasLandmarks = true;
                hasLatestEmbedding = true;

                MatchResult result;
                {
                    std::lock_guard<std::mutex> lock(state.databaseMutex);
                    result = findBestMatch(latestEmbedding.data(), database, 0.6f);
                    if (result.matched)
                    {
                        const FaceRecord *record = database.findById(result.recordId);
                        if (record != nullptr)
                        {
                            annotation.label = record->name;
                        }
                    }
                }

                if (result.matched && !annotation.label.empty())
                {
                    if (firstFaceRecordId == -2)
                    {
                        firstFaceRecordId = result.recordId;
                        firstFaceName = annotation.label;
                    }
                }
                else
                {
                    annotation.label = "No Match";
                    if (firstFaceRecordId == -2)
                    {
                        firstFaceRecordId = -1;
                        firstFaceName = "No Match";
                    }
                }
            }
            annotations.push_back(std::move(annotation));
        }

        if (firstFaceRecordId != lastReportedRecordId)
        {
            if (firstFaceRecordId == -1)
            {
                std::cout << "No match found\n";
            }
            else if (firstFaceRecordId >= 0)
            {
                std::cout << "Matched with ID: " << firstFaceRecordId << "\n";
            }
            if (firstFaceRecordId != -2)
            {
                eventLog.append(std::chrono::system_clock::now().time_since_epoch().count(),
                                firstFaceRecordId, firstFaceName.c_str());
            }
            lastReportedRecordId = firstFaceRecordId;
        }

        {
            std::lock_guard<std::mutex> lock(state.resultMutex);
            state.annotations = std::move(annotations);
            state.latestEmbedding = latestEmbedding;
            state.hasLatestEmbedding = hasLatestEmbedding;
        }
    }
}

int main()
{
    std::error_code filesystemError;
    const std::filesystem::path dataDirectory(DATA_DIR);
    const std::string databasePath = (dataDirectory / "faces.db").string();
    std::filesystem::create_directories(dataDirectory, filesystemError);
    if (filesystemError)
    {
        std::cerr << "Failed to create data directory: " << filesystemError.message() << "\n";
        return 1;
    }

    Capture capture;
    if (!capture.open(0))
    {
        std::cerr << "Failed to open camera\n";
        return 1;
    }

    Detector detector;
    if (!detector.load(std::string(MODELS_DIR) + "/haarcascades/haarcascade_frontalface_default.xml"))
    {
        std::cerr << "Failed to load cascade\n";
        capture.release();
        return 1;
    }

    Encoder encoder;
    if (!encoder.load(std::string(MODELS_DIR) + "/shape_predictor_68_face_landmarks.dat",
                      std::string(MODELS_DIR) + "/dlib_face_recognition_resnet_model_v1.dat"))
    {
        std::cerr << "Failed to load encoder\n";
        capture.release();
        return 1;
    }

    FaceDatabase database;
    if (!loadDatabase(database, databasePath.c_str()))
    {
        if (std::filesystem::exists(databasePath, filesystemError) || filesystemError)
        {
            std::cerr << "Failed to load existing face database\n";
            capture.release();
            return 1;
        }
        std::cout << "No face database found; starting with an empty database\n";
    }

    EventLog eventLog;
    SharedState state;
    std::thread captureThread(captureLoop, std::ref(capture), std::ref(state));
    std::thread recognitionThread(recognitionLoop, std::ref(state), std::ref(detector),
                                  std::ref(encoder), std::ref(database), std::ref(eventLog));

    std::uint64_t displayedVersion = 0;
    auto resumeRecognition = [&state]
    {
        {
            std::lock_guard<std::mutex> lock(state.frameMutex);
            state.recognitionPaused = false;
        }
        state.frameCondition.notify_all();
    };

    while (state.running)
    {
        cv::Mat frame;
        std::uint64_t currentVersion = 0;
        {
            std::lock_guard<std::mutex> lock(state.frameMutex);
            if (state.frameVersion > displayedVersion)
            {
                frame = state.latestFrame.clone();
                currentVersion = state.frameVersion;
            }
        }

        if (!frame.empty())
        {
            displayedVersion = currentVersion;
            std::vector<FaceAnnotation> annotations;
            {
                std::lock_guard<std::mutex> lock(state.resultMutex);
                annotations = state.annotations;
            }

            for (const FaceAnnotation &annotation : annotations)
            {
                if (annotation.hasLandmarks)
                {
                    for (const cv::Point &point : annotation.landmarks)
                    {
                        cv::circle(frame, point, 2, cv::Scalar(0, 0, 255), -1);
                    }
                }
                cv::rectangle(frame, annotation.rect, cv::Scalar(0, 255, 0), 2);
                cv::putText(frame, annotation.label, cv::Point(annotation.rect.x, annotation.rect.y - 10),
                            cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 0, 0), 2);
            }

            cv::imshow("fac-rec", frame);
        }

        int key = cv::waitKey(1);
        if (key == 27)
        {
            break;
        }
        if (key == 'e')
        {
            {
                std::unique_lock<std::mutex> lock(state.frameMutex);
                state.recognitionPaused = true;
                state.frameCondition.notify_all();
                state.frameCondition.wait(lock, [&state]
                                          { return state.recognitionWorkerPaused || !state.running; });
            }

            std::array<float, Encoder::kEmbeddingSize> embedding{};
            bool canEnroll = false;
            {
                std::lock_guard<std::mutex> lock(state.resultMutex);
                canEnroll = state.hasLatestEmbedding;
                if (canEnroll)
                {
                    embedding = state.latestEmbedding;
                }
            }
            if (!canEnroll)
            {
                std::cout << "No face detected. Press 'e' to enroll a new face when a face is detected.\n";
                resumeRecognition();
                continue;
            }

            std::string name;
            std::cout << "Recognition paused. Type the new name in this terminal: " << std::flush;
            if (!(std::cin >> name))
            {
                state.running = false;
                state.frameCondition.notify_all();
                break;
            }

            FaceRecord newRecord;
            std::lock_guard<std::mutex> lock(state.databaseMutex);
            newRecord.id = 0;
            bool idAvailable = true;
            while (database.findById(newRecord.id) != nullptr)
            {
                if (newRecord.id == std::numeric_limits<int>::max())
                {
                    idAvailable = false;
                    break;
                }
                ++newRecord.id;
            }
            if (!idAvailable)
            {
                std::cerr << "No available record IDs\n";
                resumeRecognition();
                continue;
            }

            std::strncpy(newRecord.name, name.c_str(), sizeof(newRecord.name) - 1);
            newRecord.name[sizeof(newRecord.name) - 1] = '\0';
            newRecord.enrolledAt = std::chrono::system_clock::now().time_since_epoch().count();
            std::memcpy(newRecord.embedding, embedding.data(), sizeof(float) * embedding.size());
            database.add(newRecord);

            if (!saveFaceDatabase(database, databasePath.c_str()))
            {
                std::cerr << "Failed to save face database\n";
            }
            else
            {
                std::cout << "Enrolled new face with ID: " << newRecord.id << "\n";
            }
            resumeRecognition();
        }
    }

    state.running = false;
    state.frameCondition.notify_all();
    captureThread.join();
    recognitionThread.join();
    capture.release();

    cv::destroyAllWindows();
    eventLog.printall();
    return 0;
}
