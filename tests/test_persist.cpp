#include "Persistence.h"
#include <iostream>
#include <cstring>

int main()
{
    FaceDatabase original;
    for (int i = 0; i < 5; ++i)
    {
        FaceRecord r;
        r.id = i;
        std::snprintf(r.name, sizeof(r.name), "person_%d", i);
        r.enrolledAt = 1000 + i;
        r.embedding[0] = static_cast<float>(i) * 1.5f;
        original.add(r);
    }

    if (!saveFaceDatabase(original, "/tmp/test.fdb"))
    {
        std::cerr << "save failed\n";
        return 1;
    }
    std::cout << "saved " << original.size() << " records\n";

    FaceDatabase loaded;
    if (!loadDatabase(loaded, "/tmp/test.fdb"))
    {
        std::cerr << "load failed\n";
        return 1;
    }
    std::cout << "loaded " << loaded.size() << " records\n";

    bool allMatch = true;
    for (int i = 0; i < loaded.size(); ++i)
    {
        const FaceRecord *o = original.at(i);
        const FaceRecord *l = loaded.at(i);
        if (o->id != l->id || std::strcmp(o->name, l->name) != 0 ||
            o->enrolledAt != l->enrolledAt || o->embedding[0] != l->embedding[0])
        {
            allMatch = false;
            std::cout << "MISMATCH at " << i << "\n";
        }
    }
    std::cout << (allMatch ? "ALL RECORDS MATCH\n" : "MISMATCH FOUND\n");

    FaceDatabase garbage;
    bool garbageResult = loadDatabase(garbage, "/tmp/does_not_exist.fdb");
    std::cout << "load of missing file returned " << garbageResult << " (expect 0)\n";

    return 0;
}
