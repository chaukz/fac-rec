#pragma once
static const int kEmbeddingSize = 128;
struct FaceRecord
{
    int id;
    char name[64];
    long long enrolledAt;
    float *embedding;
    FaceRecord();
    FaceRecord(const FaceRecord &other);
    FaceRecord &operator=(const FaceRecord &other);
    ~FaceRecord();
};