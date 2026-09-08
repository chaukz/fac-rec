#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
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

    cv::Mat frame;
    while (cap.readFrame(frame))
    {
        dlib::cv_image<dlib::bgr_pixel> dlibImg(frame);

        for (const cv::Rect &faceRect : detector.detect(frame))
        {
            cv::rectangle(frame, faceRect, cv::Scalar(0, 255, 0), 2);

            dlib::rectangle dlibRect(faceRect.x, faceRect.y,
                                     faceRect.x + faceRect.width - 1,
                                     faceRect.y + faceRect.height - 1);
            dlib::full_object_detection shape = sp(dlibImg, dlibRect);

            for (unsigned long i = 0; i < shape.num_parts(); ++i)
            {
                cv::Point pt(shape.part(i).x(), shape.part(i).y());
                cv::circle(frame, pt, 2, cv::Scalar(0, 0, 255), -1);
            }
        }
        cv::imshow("green box + red dots", frame);
        if (cv::waitKey(1) == 27)
            break;
    }
    return 0;
}