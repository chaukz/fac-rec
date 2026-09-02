#pragma once

struct FaceRecord
{
    int id;
    char name[64];
    long long enrolledAt;
    float *embedding;

    FaceRecord();
    ~FaceRecord();
};
