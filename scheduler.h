#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

enum class ProcessState
{
    NEW,
    READY,
    RUNNING,
    TERMINATED
};

struct PCB
{
    int schedulerPid;
    string command;

    int arrivalTime;
    int burstTime;
    int remainingTime;
    int priority;

    int firstStart;
    int completion;

    uint64_t sequence;

    ProcessState state;
};

class Scheduler
{
private:
    vector<PCB> processes;

    int nextPid;
    uint64_t nextSequence;

    int quantum;
    int delayMs;

    enum class Algorithm
    {
        ROUND_ROBIN,
        PRIORITY
    };

    Algorithm algorithm;

    void runRoundRobin();
    void runPriority();

    void displayMetrics() const;
    void sleepOneUnit() const;

public:
    Scheduler();

    void setRoundRobin(int quantumValue, int delay);
    void setPriority(int delay);

    void addProcess(
        int burst,
        int priority,
        int arrival,
        const string &command
    );

    void showProcesses() const;
    void run();
    void showMetrics() const;
    void clear();
};

#endif