#include <opencv2/opencv.hpp>

#include <iostream>
#include <chrono>
#include <cstring>

#include "Capture.h"
#include "Detector.h"
#include "Encoder.h"
#include "Persistence.h"
#include "EventLog.h"
#include "FaceDatabase.h"
#include "Matcher.h"

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
    loadDatabase(db, "data/faces.db");

    EventLog eventLog;
    float lastEmbedding[128] = {0};
    bool hasFace = false;
    cv::Mat frame;
    while (cap.readFrame(frame))
    {
        for (const cv::Rect &face : detector.detect(frame))
        {
            float embedding[128];
            if (encoder.encode(frame, face, embedding))
            {
                hasFace = true;
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
                newRecord.embedding = new float[128];
                memcpy(newRecord.embedding, lastEmbedding, sizeof(float) * 128);
                db.add(newRecord);
                saveFaceDatabase(db, "data/faces.db");
                std::cout << "Enrolled new face with ID: " << newRecord.id << "\n";
            }
        }
    }
    return 0;
}
