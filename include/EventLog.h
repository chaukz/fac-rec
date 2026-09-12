#ifndef EVENTLOG_H
#define EVENTLOG_H

struct LogNOde
{
    long long timestampe;
    int recordId;  // indicates false / no match
    char name[64]; // unkown if recordId == -1
    LogNode *next; // linked list pointer
};

class EventLog
{
public:
    EventLog();
    ~EventLog();
    void append(long long timestamp, int recordId, const char *name);
    void printall() const;
    int count() const;

private:
    LogNode *head;
    int size;
};

#endif // EVENTLOG_H