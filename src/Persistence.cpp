#include "Persistence.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

namespace
{
    const char kMagic[4] = {'F', 'S', 'D', 'B'};
    const int kVersion = 1;
}

bool saveFaceDatabase(const FaceDatabase &db, const char *path)
{
    if (path == nullptr || db.size() < 0)
    {
        return false;
    }
    const std::string temporaryPath = std::string(path) + ".tmp";
    FILE *file = std::fopen(temporaryPath.c_str(), "wb");
    if (!file)
    {
        return false;
    }

    int count = db.size();

    bool ok = std::fwrite(kMagic, sizeof(char), 4, file) == 4 &&
              std::fwrite(&kVersion, sizeof(int), 1, file) == 1 &&
              std::fwrite(&count, sizeof(int), 1, file) == 1;

    for (int i = 0; ok && i < count; ++i)
    {
        const FaceRecord *rec = db.at(i);
        ok = rec != nullptr &&
             std::fwrite(&rec->id, sizeof(int), 1, file) == 1 &&
             std::fwrite(rec->name, sizeof(char), sizeof(rec->name), file) == sizeof(rec->name) &&
             std::fwrite(&rec->enrolledAt, sizeof(long long), 1, file) == 1 &&
             std::fwrite(rec->embedding, sizeof(float), kEmbeddingSize, file) == kEmbeddingSize;
    }

    bool closed = std::fclose(file) == 0;
    if (!ok || !closed)
    {
        std::remove(temporaryPath.c_str());
        return false;
    }
    if (std::rename(temporaryPath.c_str(), path) != 0)
    {
        std::remove(temporaryPath.c_str());
        return false;
    }
    return true;
}

bool loadDatabase(FaceDatabase &db, const char *path)
{
    if (path == nullptr)
    {
        return false;
    }
    FILE *file = std::fopen(path, "rb");
    if (!file)
    {
        return false;
    }

    if (std::fseek(file, 0, SEEK_END) != 0)
    {
        std::fclose(file);
        return false;
    }
    long fileSize = std::ftell(file);
    constexpr long headerSize = 4 + 2 * sizeof(int);
    constexpr long recordSize = sizeof(int) + 64 + sizeof(long long) + kEmbeddingSize * sizeof(float);
    if (fileSize < headerSize || std::fseek(file, 0, SEEK_SET) != 0)
    {
        std::fclose(file);
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
    if (std::fread(&count, sizeof(int), 1, file) != 1 || count < 0 ||
        count > (fileSize - headerSize) / recordSize ||
        headerSize + static_cast<long>(count) * recordSize != fileSize)
    {
        std::fclose(file);
        return false;
    }

    FaceDatabase loaded;
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

        if (rec.id < 0 || std::memchr(rec.name, '\0', sizeof(rec.name)) == nullptr ||
            loaded.findById(rec.id) != nullptr)
        {
            std::fclose(file);
            return false;
        }
        for (int dimension = 0; dimension < kEmbeddingSize; ++dimension)
        {
            if (!std::isfinite(rec.embedding[dimension]))
            {
                std::fclose(file);
                return false;
            }
        }

        loaded.add(rec);
    }

    if (std::fclose(file) != 0)
    {
        return false;
    }
    db.swap(loaded);
    return true;
}
