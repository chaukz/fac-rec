#include "Persistence.h"
#include <cstdio>
#include <cstring>

namespace
{
    const char kMagic[4] = {'F', 'S', 'D', 'B'};
    const int kVersion = 1;
}

bool saveFaceDatabase(const FaceDatabase &db, const char *path)
{
    FILE *file = std::fopen(path, "wb");
    if (!file)
    {
        return false;
    }

    int count = db.size();

    std::fwrite(kMagic, sizeof(char), 4, file);
    std::fwrite(&kVersion, sizeof(int), 1, file);
    std::fwrite(&count, sizeof(int), 1, file);

    for (int i = 0; i < count; ++i)
    {
        const FaceRecord *rec = db.at(i);
        std::fwrite(&rec->id, sizeof(int), 1, file);
        std::fwrite(rec->name, sizeof(char), 64, file);
        std::fwrite(&rec->enrolledAt, sizeof(long long), 1, file);
        std::fwrite(rec->embedding, sizeof(float), kEmbeddingSize, file);
    }

    std::fclose(file);
    return true;
}

bool loadDatabase(FaceDatabase &db, const char *path)
{
    FILE *file = std::fopen(path, "rb");
    if (!file)
    {
        return false;
    }

    char magic[4];
    if (std::fread(magic, sizeof(char), 4, file) != 4 ||
        std::memcmp(magic, kMagic, 4) != 0)
    {
        std::fclose(file);
        return false;
    }

    int version = 0;
    if (std::fread(&version, sizeof(int), 1, file) != 1 || version != kVersion)
    {
        std::fclose(file);
        return false;
    }

    int count = 0;
    if (std::fread(&count, sizeof(int), 1, file) != 1)
    {
        std::fclose(file);
        return false;
    }

    for (int i = 0; i < count; ++i)
    {
        FaceRecord rec;

        bool ok = std::fread(&rec.id, sizeof(int), 1, file) == 1 &&
                  std::fread(rec.name, sizeof(char), 64, file) == 64 &&
                  std::fread(&rec.enrolledAt, sizeof(long long), 1, file) == 1 &&
                  std::fread(rec.embedding, sizeof(float), kEmbeddingSize, file) == (size_t)kEmbeddingSize;

        if (!ok)
        {
            std::fclose(file);
            return false;
        }

        db.add(rec);
    }

    std::fclose(file);
    return true;
}