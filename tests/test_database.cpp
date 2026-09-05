#include "FaceRecord.h"
#include "FaceDatabase.h"
#include <iostream>

int main()
{
    FaceDatabase db;
    for (int i = 0; i < 12; i++)
    {
        FaceRecord r;
        r.id = i;
        r.embedding[0] = static_cast<float>(i);
        db.add(r);
        std::cout << "added" << i << ", size = " << db.size() << "\n";
    }
    std::cout << "done, final size = " << db.size() << "\n";
    return 0;
}