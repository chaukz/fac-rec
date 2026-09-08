#include "FaceRecord.h"
#include "FaceDatabase.h"
#include "Matcher.h"
#include <cstring>
#include <iostream>

FaceRecord makeRecord(int id, const char *name, float fillValue)
{
    FaceRecord record;
    record.id = id;
    std::strncpy(record.name, name, sizeof(record.name) - 1);
    for (int i = 0; i < kEmbeddingSize; ++i)
    {
        record.embedding[i] = fillValue;
    }
    return record;
}

int main()
{
    FaceDatabase db;

    db.add(makeRecord(1, "Alice", 0.0f));
    db.add(makeRecord(2, "Bob", 5.0f));
    db.add(makeRecord(3, "Carol", 10.0f));

    float probe[kEmbeddingSize];
    for (int i = 0; i < kEmbeddingSize; ++i)
    {
        probe[i] = 0.02f;
    }

    MatchResult result = findBestMatch(probe, db, 0.6f);

    std::cout << "matched: " << (result.matched ? "true" : "false") << "\n";
    std::cout << "recordId: " << result.recordId << "\n";
    std::cout << "distance: " << result.distance << "\n";

    return 0;
}