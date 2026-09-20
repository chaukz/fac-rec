#include <opencv2/opencv.hpp>
#include <iostream>
#include "Capture.h"
#include "Detector.h"
#include "Encoder.h"
#include "Persistence.h"
#include "EventLog.h"
#include "FaceDatabase.h"

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

    cv::Mat frame;
    while (cap.readFrame(frame))
    {
        for (const cv::Rect &face : detector.detect(frame))
        {
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
        }
        cv::imshow("fac-rec", frame);
        if (cv::waitKey(1) == 27)
            break;
    }

    return 0;
}