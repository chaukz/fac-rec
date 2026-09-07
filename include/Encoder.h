#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <memory>

class Encoder
{
public:
    static constexpr int kEmbeddingSize = 128;

    Encoder();
    ~Encoder();
    bool load(const std::string &landmarkModel, const std::string &resnetModel);
    bool encode(const cv::Mat &frame, const cv::Rect &faceRect, float *out);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};