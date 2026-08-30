#include <iostream>
#include <opencv2/opencv.hpp>
#include "Capture.h"

int main() {
    Capture cap;

    if (!cap.open(0)) {
        std::cerr << "Failed to open camera\n";
        return 1;
    }

    cv::Mat frame;
    while (cap.readFrame(frame)) {
        cv::imshow("fac-rec", frame);
        if (cv::waitKey(1) == 27)
            break;
    }

    return 0;
}
