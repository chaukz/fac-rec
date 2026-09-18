#include "EventLog.h"
#include <cstring>
#include <cstdio>

EventLog::EventLog() : head(nullptr), size(0) {}
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
    LogNode *newNode = new LogNode;                               // Allocate memory for a new LogNode
    newNode->timestamp = timestamp;                               // Set the timestamp of the new node
    newNode->recordId = recordId;                                 // Set the recordId of the new node
    std::strncpy(newNode->name, name, sizeof(newNode->name) - 1); // Copy the name into the new node, ensuring it doesn't exceed the buffer size
    newNode->name[sizeof(newNode->name) - 1] = '\0';              // Ensure null-termination
    newNode->next = nullptr;                                      // Set the next pointer of the new node to nullptr
    if (head == nullptr)                                          // If the list is empty, set the new node as the head
    {
        head = newNode; // Set the head to the new node
    }
    else
    {
        LogNode *current = head;         // Start from the head of the list
        while (current->next != nullptr) // Traverse to the end of the list
        {
            current = current->next; // Move to the next node
        }
        current->next = newNode; // Link the new node at the end of the list
    }
    size++; // Increment the size of the list
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