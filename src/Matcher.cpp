#include "Matcher.h"
#include <cmath>

float euclideanDistance(const float *a, const float *b, int length)
{
    float sum = 0.0f;
    for (int i = 0; i < length; i++)
    {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

MatchResult findBestMatch(const float *probe, const FaceDatabase &db, float threshold)
{
    float bestDistance = 1e9F;
    int bestId = -1;

    for (int i = 0; i < db.size(); i++)
    {
        const FaceRecord *record = db.at(i);
        if (record == nullptr)
            continue;

        float dist = euclideanDistance(probe, record->embedding, kEmbeddingSize);

        if (dist < bestDistance)
        {
            bestDistance = dist;
            bestId = record->id;
        }
    }
    MatchResult result;
    result.recordId = bestId;
    result.distance = bestDistance;
    result.matched = (bestId != -1 && bestDistance < threshold);
    return result;
}