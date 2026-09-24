#include <opencv2/opencv.hpp>

#include <iostream>
#include <chrono>
#include <cstring>
#include <mutex>
#include <thread>
#include <atomic>

#include "Capture.h"
#include "Detector.h"
#include "Encoder.h"
#include "Persistence.h"
#include "EventLog.h"
#include "FaceDatabase.h"
#include "Matcher.h"

struct SharedState
{
    cv::Mat sharedFrame;
    bool frameReady = false;
    std::mutex frameMutex;
    std::atomic<bool> running{true};
};

void captureLoop(Capture &cap, SharedState &state)
{
    while (state.running)
    {
        cv::Mat frame;
        if (cap.readFrame(frame))
        {
            std::lock_guard<std::mutex> lock(state.frameMutex);
            state.sharedFrame = frame.clone();
            state.frameReady = true;
        }
        else
        {
            std::cerr << "Failed to read frame from camera\n";
            state.running = false;
        }
    }
}

int main()
{
    Capture cap;
    if (!cap.open(0))
    {
        std::cerr << "Failed to open camera\n";
        return 1;
    }

    Detector detector;
    if (!detector.load(std::string(MODELS_DIR) + "/haarcascades/haarcascade_frontalface_default.xml"))
    {
        std::cerr << "Failed to load cascade\n";
        return 1;
    }

    Encoder encoder;
    if (!encoder.load(std::string(MODELS_DIR) + "/shape_predictor_68_face_landmarks.dat", std::string(MODELS_DIR) + "/dlib_face_recognition_resnet_model_v1.dat"))
    {
        std::cerr << "Failed to load encoder\n";
        return 1;
    }

    FaceDatabase db;
    if (!loadDatabase(db, "data/faces.db"))
    {
        std::cerr << "Failed to load face database\n";
    }

    EventLog eventLog;
    float lastEmbedding[128] = {0};
    bool hasFace = false;
    cv::Mat frame;

    SharedState state;
    std::thread captureThread(captureLoop, std::ref(cap), std::ref(state));

    while (state.running)
    {
        {
            std::lock_guard<std::mutex> lock(state.frameMutex);
            if (state.frameReady)
            {
                frame = state.sharedFrame.clone();
                state.frameReady = false;
            }
            else
            {
                continue;
            }
        }

        if (frame.empty())
        {
            std::cerr << "Empty frame received\n";
            continue;
        }

        hasFace = false;
        {
            for (const cv::Rect &face : detector.detect(frame))
            {
                cv::Point landmarks[68];
                float embedding[128];
                if (encoder.encode(frame, face, embedding, landmarks))
                {
                    hasFace = true;
                    for (int i = 0; i < 68; ++i)
                    {
                        cv::circle(frame, landmarks[i], 2, cv::Scalar(0, 0, 255), -1);
                    }
                    memcpy(lastEmbedding, embedding, sizeof(float) * 128);

                    MatchResult result = findBestMatch(embedding, db, 0.6f);
                    if (result.matched)
                    {
                        std::cout << "Matched with ID: " << result.recordId << ", Distance: " << result.distance << "\n";
                        eventLog.append(std::chrono::system_clock::now().time_since_epoch().count(), result.recordId, db.at(result.recordId)->name);
                    }
                    else
                    {
                        std::cout << "No match found\n";
                        eventLog.append(std::chrono::system_clock::now().time_since_epoch().count(), -1, "No Match");
                    }
                }
                cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
            }
            cv::imshow("fac-rec", frame);

            int key = cv::waitKey(1);
            if (key == 27)
            {
                break;
            }

            else if (key == 'e')
            {
                if (!hasFace)
                {
                    std::cout << "No face detected. Press 'e' to enroll a new face when a face is detected.\n";
                    // continue;
                }
                else
                {
                    // enroll new face
                    std::string name;
                    std::cout << "Enter name for new face: ";
                    std::cin >> name;
                    FaceRecord newRecord;
                    newRecord.id = db.size();
                    strncpy(newRecord.name, name.c_str(), sizeof(newRecord.name) - 1);
                    newRecord.enrolledAt = std::chrono::system_clock::now().time_since_epoch().count();
                    // newRecord.embedding = new float[128];
                    memcpy(newRecord.embedding, lastEmbedding, sizeof(float) * 128);
                    db.add(newRecord);

                    if (!saveFaceDatabase(db, "data/faces.db"))
                    {
                        std::cerr << "Failed to save face database\n";
                    }
                    else
                    {
                        std::cout << "Enrolled new face with ID: " << newRecord.id << "\n";
                    }
                }
            }
        }
    }
    state.running = false;
    captureThread.join();

    cv::destroyAllWindows();
    EventLog.printall();
    return 0;
}
