#pragma once
#include <opencv2/opencv.hpp>

class Capture
{
public:
    bool open(int deviceIndex);
    bool readFrame(cv::Mat &out);
    void release()
    {
        cap_.release();
    }

private:
    cv::VideoCapture cap_;
};