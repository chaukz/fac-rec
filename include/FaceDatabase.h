#pragma once
#include <FaceDatabase.h>
#include <FaceRecord.h>

class FaceDatabase
{
public:
    FaceDatabase();
    ~FaceDatabase();

    void add(const FaceRecord &rec);       // Add a new face record to the database
    const FaceRecord *at(int index) const; // Get a face record by index
    int size() const { return count_; }    // Get the number of face records in the database
    bool removeById(int id);               // Remove a face record by its ID, returns true if removed, false if not found

private:
    void grow(); // Increase the capacity of the database when needed

    FaceRecord **records_; // Pointer to an array of pointers to FaceRecord objects
    int count_;            // Current number of face records in the database
    int capacity_;         // Current capacity of the database
};