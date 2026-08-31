#include <opencv2/opencv.hpp>
#include <iostream>
#include "Capture.h"
#include "Detector.h"

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