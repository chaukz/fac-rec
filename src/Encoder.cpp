#include "Encoder.h"
#include <dlib/dnn.h>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_transforms.h>
#include <iostream>

using namespace dlib;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<alevel0<alevel1<alevel2<alevel3<alevel4<max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2,input_rgb_image_sized<150>>>>>>>>>>>>>;

struct Encoder::Impl {
    shape_predictor sp;
    anet_type net;
};

Encoder::Encoder() : impl_(std::make_unique<Impl>()) {}
Encoder::~Encoder() = default;

bool Encoder::load(const std::string& landmarkModel, const std::string& resnetModel)
{
    try {
        deserialize(landmarkModel) >> impl_->sp;
        deserialize(resnetModel) >> impl_->net;
    } catch (const serialization_error& e) {
        std::cerr << "Encoder::load failed: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool Encoder::encode(const cv::Mat& frame, const cv::Rect& faceRect, float* out)
{
    cv_image<bgr_pixel> dlibImg(frame);
    dlib::rectangle dlibRect(faceRect.x, faceRect.y, faceRect.x + faceRect.width - 1, faceRect.y + faceRect.height - 1);

    full_object_detection shape = impl_->sp(dlibImg, dlibRect);

    matrix<rgb_pixel> faceChip;
    extract_image_chip(dlibImg, get_face_chip_details(shape, 150, 0.25), faceChip);

    matrix<float,0,1> descriptor = impl_->net(faceChip);

    for (int i = 0; i < kEmbeddingSize; ++i) {
        out[i] = descriptor(i);
    }
    return true;
}
