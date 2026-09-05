#include <FaceDatabase.h>

FaceDatabase::FaceDatabase()
    : records_(nullptr), count_(0), capacity_(0)
{
}

FaceDatabase::~FaceDatabase()
{
    for (int i = 0; i < count_; ++i)
    {
        delete records_[i];
    }
    delete[] records_;
}

void FaceDatabase::grow()
{
    int newCapacity = (capacity_ == 0) ? 4 : capacity_ * 2;
    FaceRecord **newRecords = new FaceRecord *[newCapacity];

    for (int i = 0; i < count_; i++)
    {
        newRecords[i] = records_[i];
    }
    delete[] records_;
    records_ = newRecords;
    capacity_ = newCapacity;
}
void FaceDatabase::add(const FaceRecord &rec)
{
    if (count_ == capacity_)
    {
        grow();
    }
    records_[count_] = new FaceRecord(rec);
    ++count_;
}
