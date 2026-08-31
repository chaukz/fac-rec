#include "Detector.h"

bool Detector::load(const std::string &cascadePath)
{
    return cascade_.load(cascadePath);
}

std::vector<cv::Rect> Detector::detect(const cv::Mat &frame)
{
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Rect> faces;
    cascade_.detectMultiScale(gray, faces);
    return faces;
}