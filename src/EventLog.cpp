#include "EventLog.h"

EventLog::EventLog() : head(nullptr), size(0) {}
LogNode *current = head;

while (current != nullptr)
{
    LogNode *next = current->next; // save the address of the next box first
    delete current;                // now safe to free this box
    current = next;                // move forward using the saved address
}
}

void Eventlog::append(long long timestamp, int recordId, const char *name)
{
    LogNode *newNode = new LogNode;
    newNode->timestamp = timestamp;
    newNode->recordId = recordId;
    std::strncpy(newNode->name, name, sizeof(newNode->name) - 1);
    newNode->name[sizeof(newNode->name) - 1] = '\0'; // Ensure null-termination
    newNode->next = nullptr;
    if (head == nullptr)
    {
        head = newNode;
    }
    else
    {
        LogNode *current = head;
        while (current->next != nullptr)
        {
            current = current->next;
        }
        current->next = newNode;
    }
    size++;
}