#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_transforms.h>
#include <iostream>
#include "Detector.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " input.jpg output_chip.jpg\n";
        return 1;
    }

    cv::Mat img = cv::imread(argv[1]);
    if (img.empty())
    {
        std::cerr << "Failed to load " << argv[1] << "\n";
        return 1;
    }

    Detector detector;
    if (!detector.load(std::string(MODELS_DIR) + "/haarcascades/haarcascade_frontalface_default.xml"))
    {
        std::cerr << "Failed to load cascade\n";
        return 1;
    }

    auto faces = detector.detect(img);
    std::cout << "faces detected in " << argv[1] << ": " << faces.size() << "\n";
    if (faces.empty())
        return 1;

    cv::Rect faceRect = faces[0];
    std::cout << "face rect: x=" << faceRect.x << " y=" << faceRect.y
              << " w=" << faceRect.width << " h=" << faceRect.height
              << "  (image is " << img.cols << "x" << img.rows << ")\n";

    cv::Mat annotated = img.clone();
    cv::rectangle(annotated, faceRect, cv::Scalar(0, 255, 0), 2);
    cv::imwrite(std::string(argv[2]) + "_detected.jpg", annotated);

    dlib::shape_predictor sp;
    try
    {
        dlib::deserialize(std::string(MODELS_DIR) + "/shape_predictor_68_face_landmarks.dat") >> sp;
    }
    catch (const dlib::serialization_error &e)
    {
        std::cerr << "Failed to load shape predictor: " << e.what() << "\n";
        return 1;
    }

    dlib::cv_image<dlib::bgr_pixel> dlibImg(img);
    dlib::rectangle dlibRect(faceRect.x, faceRect.y,
                             faceRect.x + faceRect.width - 1,
                             faceRect.y + faceRect.height - 1);
    dlib::full_object_detection shape = sp(dlibImg, dlibRect);

    dlib::matrix<dlib::rgb_pixel> chip;
    dlib::extract_image_chip(dlibImg, dlib::get_face_chip_details(shape, 150, 0.25), chip);

    cv::Mat chipRgb = dlib::toMat(chip);
    cv::Mat chipBgr;
    cv::cvtColor(chipRgb, chipBgr, cv::COLOR_RGB2BGR);
    cv::imwrite(argv[2], chipBgr);
    std::cout << "saved chip to " << argv[2] << "\n";

    return 0;
}