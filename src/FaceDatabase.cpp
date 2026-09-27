#include <FaceDatabase.h>

FaceDatabase::FaceDatabase() // Initialize the FaceDatabase with default values
    : records_(nullptr), count_(0), capacity_(0) // Start with no records and zero capacity
{
}

FaceDatabase::~FaceDatabase()
{
    for (int i = 0; i < count_; ++i) // Delete each FaceRecord object to free memory
    {
        delete records_[i];
    }
    delete[] records_;
}

void FaceDatabase::grow()
{
    int newCapacity = (capacity_ == 0) ? 4 : capacity_ * 2; // Double the capacity or start with 4 if it was zero
    FaceRecord **newRecords = new FaceRecord *[newCapacity]; // Allocate new array of pointers to FaceRecord

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

const FaceRecord *FaceDatabase::at(int index) const
{
    if (index < 0 || index >= count_)
    {
        return nullptr;
    }
    return records_[index];
}

const FaceRecord *FaceDatabase::findById(int id) const
{
    for (int i = 0; i < count_; ++i)
    {
        if (records_[i]->id == id)
        {
            return records_[i];
        }
    }
    return nullptr;
}

void FaceDatabase::swap(FaceDatabase &other) noexcept
{
    FaceRecord **records = records_;
    records_ = other.records_;
    other.records_ = records;

    int count = count_;
    count_ = other.count_;
    other.count_ = count;

    int capacity = capacity_;
    capacity_ = other.capacity_;
    other.capacity_ = capacity;
}

bool FaceDatabase::removeById(int id)
{
    for (int i = 0; i < count_; ++i)
    {
        if (records_[i]->id == id)
        {
            delete records_[i]; // Free the memory of the FaceRecord being removed
            for (int j = i; j < count_ - 1; ++j)
            {
                records_[j] = records_[j + 1]; // Shift remaining records to fill the gap
            }
            --count_;
            return true; // Successfully removed the record
        }
    }
    return false; // Record with the given ID was not found
}
