#include "EventLog.h"
#include <iostream>

int main()
{
    EventLog log;

    log.append(1700000001, 1, "Alice");
    log.append(1700000002, -1, "unknown");
    log.append(1700000003, 2, "Bob");

    std::cout << "count: " << log.count() << "\n";
    log.printall();

    return 0;
}
