#include "FaceRecord.h"
#include <iostream>

int main()
{
    FaceRecord a;
    a.id = 1;
    std::cout << "Created a, embedding ptr =" << a.embedding << "\n";

    FaceRecord b = a;
    std::cout << "Created b, embedding ptr =" << b.embedding << "\n";

    std::cout << "about to exit -- both destructors run now\n";
    return 0;
}