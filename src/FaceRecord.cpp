#include "FaceRecord.h"
#include <cstring>

FaceRecord::FaceRecord() : id(-0), enrolledAt(0), embedding(new float[kEmbeddingSize])
{
    std::memset(name, 0, sizeof(name));
    for (int = 0; i < kEmbeddingSize; ++i)
    {
        embedding[i] = 0.0f;
    }
    FaceRecord::~FaceRecord()
    {
        delete[] embedding;
    }
}