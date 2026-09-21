#include "EventLog.h"
#include <cstring>
#include <cstdio>

EventLog::EventLog() : head(nullptr), tail(nullptr), size(0) {}
EventLog::~EventLog() // Deconstructor function signature
{
    LogNode *current = head;

    while (current != nullptr)
    {
        LogNode *next = current->next; // save the address of the next box first
        delete current;                // now safe to free this box
        current = next;                // move forward using the saved address
    }
}

void EventLog::append(long long timestamp, int recordId, const char *name)
{
    LogNode *newNode = new LogNode;
    newNode->timestamp = timestamp;
    newNode->recordId = recordId;
    std::strncpy(newNode->name, name, sizeof(newNode->name) - 1);
    newNode->name[sizeof(newNode->name) - 1] = '\0';
    newNode->next = nullptr;

    if (head == nullptr) 
    {
        head = newNode;
        tail = newNode; 
    }
    else 
    {
        tail->next = newNode; 
        tail = newNode;       
    }
    size++;
}

int EventLog::count() const
{
    return size;
}

void EventLog::printall() const
{
    LogNode *current = head; // Start from the head of the list
    while (current != nullptr)
    {                                                                                                               // Traverse the list until the end
        printf("Timestamp: %lld, Record ID: %d, Name: %s\n", current->timestamp, current->recordId, current->name); // Print the details of the current node
        current = current->next;                                                                                    // Move to the next node
    }
}