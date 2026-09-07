#include <opencv2/opencv.hpp>
#include <iostream>
#include "Capture.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " output.jpg\n";
        return 1;
    }

    Capture cap;
    if (!cap.open(0))
    {
        std::cerr << "Failed to open camera\n";
        return 1;
    }

    cv::Mat frame;
    while (cap.readFrame(frame))
    {
        cv::imshow("press s to save, ESC to quit", frame);
        int key = cv::waitKey(1);
        if (key == 27)
            break;
        if (key == 's')
        {
            cv::imwrite(argv[1], frame);
            std::cout << "saved " << argv[1] << "\n";
            break;
        }
    }
    return 0;
}