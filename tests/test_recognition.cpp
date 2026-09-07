#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include "Detector.h"
#include "Encoder.h"

float euclideanDistance(const float *a, const float *b, int length)
{
    float sum = 0.0f;
    for (int i = 0; i < length; ++i)
    {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

bool encodeImageFile(const std::string &path, Detector &detector, Encoder &encoder, float *out)
{
    cv::Mat img = cv::imread(path);
    if (img.empty())
    {
        std::cerr << "Failed to load image: " << path << "\n";
        return false;
    }
    auto faces = detector.detect(img);
    if (faces.empty())
    {
        std::cerr << "No face found in: " << path << "\n";
        return false;
    }
    return encoder.encode(img, faces[0], out);
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " photo1.jpg photo2.jpg photo3.jpg\n";
        std::cerr << "  photo1, photo2: two photos of the SAME person\n";
        std::cerr << "  photo3: a photo of a DIFFERENT person\n";
        return 1;
    }

    Detector detector;
    if (!detector.load(std::string(MODELS_DIR) + "/haarcascades/haarcascade_frontalface_default.xml"))
    {
        std::cerr << "Failed to load cascade\n";
        return 1;
    }

    Encoder encoder;
    if (!encoder.load(std::string(MODELS_DIR) + "/shape_predictor_68_face_landmarks.dat",
                      std::string(MODELS_DIR) + "/dlib_face_recognition_resnet_model_v1.dat"))
    {
        std::cerr << "Failed to load encoder models\n";
        return 1;
    }

    float emb1[Encoder::kEmbeddingSize], emb2[Encoder::kEmbeddingSize], emb3[Encoder::kEmbeddingSize];
    if (!encodeImageFile(argv[1], detector, encoder, emb1))
        return 1;
    if (!encodeImageFile(argv[2], detector, encoder, emb2))
        return 1;
    if (!encodeImageFile(argv[3], detector, encoder, emb3))
        return 1;

    std::cout << "distance (same person)      = " << euclideanDistance(emb1, emb2, Encoder::kEmbeddingSize) << "  (expect < 0.6)\n";
    std::cout << "distance (different person) = " << euclideanDistance(emb1, emb3, Encoder::kEmbeddingSize) << "  (expect > 0.6)\n";
    return 0;
}