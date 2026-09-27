#include "FaceRecord.h"
#include <cstring>

FaceRecord::FaceRecord() : id(0), enrolledAt(0), embedding(new float[kEmbeddingSize])
{
    std::memset(name, 0, sizeof(name));
    for (int i = 0; i < kEmbeddingSize; ++i)
    {
        embedding[i] = 0.0f;
    }
}

FaceRecord::FaceRecord(const FaceRecord &other)
    : id(other.id), enrolledAt(other.enrolledAt), embedding(new float[kEmbeddingSize])
{
    std::memcpy(name, other.name, sizeof(name));
    std::memcpy(embedding, other.embedding, kEmbeddingSize * sizeof(float));
}

FaceRecord &FaceRecord::operator=(const FaceRecord &other)
{
    if (this == &other)
    {
        return *this;
    }

    float *replacement = new float[kEmbeddingSize];
    std::memcpy(replacement, other.embedding, kEmbeddingSize * sizeof(float));
    id = other.id;
    enrolledAt = other.enrolledAt;
    std::memcpy(name, other.name, sizeof(name));
    delete[] embedding;
    embedding = replacement;

    return *this;
}

FaceRecord::~FaceRecord()
{
    delete[] embedding;
}
