#ifndef MATCHER_H
#define MATCHER_H

#include "FaceDatabase.h"

struct MatchResult
{
    bool matched;
    int recordId;
    float distance;
};
 
float euclideanDistance(const float *a, const float *b, int length);

MatchResult findBestMatch(const float *probe, const FaceDatabase &db, float threshold);

#endif