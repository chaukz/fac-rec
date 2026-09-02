#include "FaceRecord.h"
#include <iostream>

int main() {
    FaceRecord a;
    a.id = 1;
    std::cout << "created a, embedding ptr = " << a.embedding << "\n";

    FaceRecord b;
    b.id = 2;
    std::cout << "created b, embedding ptr = " << b.embedding << "\n";

    b = a;   // ASSIGNMENT, not construction -- different code path
    std::cout << "assigned a into b, embedding ptr = " << b.embedding << "\n";

    std::cout << "about to exit -- both destructors run now\n";
    return 0;
}