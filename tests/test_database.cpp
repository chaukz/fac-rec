#include "FaceDatabase.h"
#include <iostream>

int main() {
    FaceDatabase db;
    for (int i = 0; i < 100; ++i) {
        FaceRecord r;
        r.id = i;
        r.embedding[0] = static_cast<float>(i);
        db.add(r);
    }
    std::cout << "size after 100 adds = " << db.size() << "\n";

    db.removeById(5);
    db.removeById(99);
    bool removedGhost = db.removeById(9999);
    std::cout << "removeById(9999) returned " << removedGhost << " (should be 0)\n";
    std::cout << "size after removals = " << db.size() << "\n";

    for (int i = 0; i < db.size(); ++i) {
        const FaceRecord* rec = db.at(i);
        std::cout << "record " << i << ": id=" << rec->id << "\n";
    }

    const FaceRecord* invalid = db.at(1000);
    std::cout << "at(1000) is " << (invalid ? "valid" : "null") << " (should be null)\n";

    return 0;
}