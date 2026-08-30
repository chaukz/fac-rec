#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class Detector
{
public:
    bool load(const std::string &cascadePath);
    std::vector<cv::Rect> detect(const cv::Mat &frame);

private:
    cv::CascadeClassifier cascade_;
};