#include "scheduler.h"

#include <iostream>
#include <iomanip>
#include <queue>
#include <algorithm>
#include <thread>
#include <chrono>

using namespace std;

Scheduler::Scheduler()
{
    nextPid = 1;
    nextSequence = 0;

    quantum = 2;
    delayMs = 0;

    algorithm = Algorithm::ROUND_ROBIN;
}

void Scheduler::setRoundRobin(int quantumValue, int delay)
{
    quantum = quantumValue;
    delayMs = delay;

    algorithm = Algorithm::ROUND_ROBIN;

    cout << "Scheduler: Round-Robin"
         << " | Quantum = " << quantum
         << " | Delay = " << delayMs
         << " ms" << endl;
}

void Scheduler::setPriority(int delay)
{
    delayMs = delay;

    algorithm = Algorithm::PRIORITY;

    cout << "Scheduler: Preemptive Priority"
         << " | Delay = " << delayMs
         << " ms" << endl;
}

void Scheduler::addProcess(
    int burst,
    int priority,
    int arrival,
    const string &command)
{
    PCB process;

    process.schedulerPid = nextPid++;
    process.command = command;

    process.arrivalTime = arrival;
    process.burstTime = burst;
    process.remainingTime = burst;

    process.priority = priority;

    process.firstStart = -1;
    process.completion = -1;

    process.sequence = nextSequence++;

    process.state = ProcessState::NEW;

    processes.push_back(process);

    cout << "Process S"
         << process.schedulerPid
         << " added." << endl;
}

void Scheduler::sleepOneUnit() const
{
    if (delayMs > 0)
    {
        this_thread::sleep_for(
            chrono::milliseconds(delayMs)
        );
    }
}

void Scheduler::runRoundRobin()
{
    if (processes.empty())
    {
        cout << "No processes available." << endl;
        return;
    }

    queue<int> readyQueue;

    vector<bool> admitted(processes.size(), false);

    int currentTime = 0;
    int completed = 0;

    cout << "\nROUND-ROBIN SCHEDULING\n";
    cout << "Quantum: " << quantum << endl;

    while (completed < static_cast<int>(processes.size()))
    {
        // Add newly arrived processes
        for (size_t i = 0; i < processes.size(); i++)
        {
            if (!admitted[i] &&
                processes[i].arrivalTime <= currentTime)
            {
                readyQueue.push(i);

                processes[i].state =
                    ProcessState::READY;

                admitted[i] = true;

                cout << "[t=" << currentTime
                     << "] S"
                     << processes[i].schedulerPid
                     << " entered READY queue."
                     << endl;
            }
        }

        // CPU idle
        if (readyQueue.empty())
        {
            cout << "[t=" << currentTime
                 << "] CPU Idle" << endl;

            sleepOneUnit();
            currentTime++;

            continue;
        }

        int index = readyQueue.front();
        readyQueue.pop();

        PCB &process = processes[index];

        process.state = ProcessState::RUNNING;

        if (process.firstStart == -1)
        {
            process.firstStart = currentTime;
        }

        cout << "[t=" << currentTime
             << "] Running S"
             << process.schedulerPid
             << " (" << process.command << ")"
             << endl;

        int executionTime =
            min(quantum, process.remainingTime);

        for (int i = 0; i < executionTime; i++)
        {
            sleepOneUnit();

            process.remainingTime--;

            currentTime++;

            // Admit processes that arrived during quantum
            for (size_t j = 0;
                 j < processes.size();
                 j++)
            {
                if (!admitted[j] &&
                    processes[j].arrivalTime <= currentTime)
                {
                    readyQueue.push(j);

                    processes[j].state =
                        ProcessState::READY;

                    admitted[j] = true;

                    cout << "[t=" << currentTime
                         << "] S"
                         << processes[j].schedulerPid
                         << " entered READY queue."
                         << endl;
                }
            }

            if (process.remainingTime == 0)
            {
                break;
            }
        }

        if (process.remainingTime == 0)
        {
            process.state =
                ProcessState::TERMINATED;

            process.completion = currentTime;

            completed++;

            cout << "[t=" << currentTime
                 << "] S"
                 << process.schedulerPid
                 << " completed."
                 << endl;
        }
        else
        {
            process.state = ProcessState::READY;

            readyQueue.push(index);

            cout << "[t=" << currentTime
                 << "] Quantum expired for S"
                 << process.schedulerPid
                 << ". Returning to READY queue."
                 << endl;
        }
    }

    displayMetrics();
}

void Scheduler::runPriority()
{
    if (processes.empty())
    {
        cout << "No processes available." << endl;
        return;
    }

    struct Compare
    {
        vector<PCB> *processes;

        bool operator()(int a, int b) const
        {
            const PCB &p1 = (*processes)[a];
            const PCB &p2 = (*processes)[b];

            if (p1.priority != p2.priority)
            {
                return p1.priority > p2.priority;
            }

            if (p1.arrivalTime != p2.arrivalTime)
            {
                return p1.arrivalTime >
                       p2.arrivalTime;
            }

            return p1.sequence > p2.sequence;
        }
    };

    priority_queue<
        int,
        vector<int>,
        Compare
    > readyQueue((Compare{&processes}));

    vector<bool> admitted(processes.size(), false);

    int currentTime = 0;
    int completed = 0;
    int running = -1;

    cout << "\nPREEMPTIVE PRIORITY SCHEDULING\n";

    while (completed < static_cast<int>(processes.size()))
    {
        // Admit new processes
        for (size_t i = 0; i < processes.size(); i++)
        {
            if (!admitted[i] &&
                processes[i].arrivalTime <= currentTime)
            {
                readyQueue.push(i);

                processes[i].state =
                    ProcessState::READY;

                admitted[i] = true;

                cout << "[t=" << currentTime
                     << "] S"
                     << processes[i].schedulerPid
                     << " arrived"
                     << " | Priority = "
                     << processes[i].priority
                     << endl;
            }
        }

        // Check for preemption
        if (running != -1 && !readyQueue.empty())
        {
            int candidate = readyQueue.top();

            if (processes[candidate].priority <
                processes[running].priority)
            {
                cout << "[t=" << currentTime
                     << "] PREEMPT S"
                     << processes[running].schedulerPid
                     << " -> S"
                     << processes[candidate].schedulerPid
                     << endl;

                processes[running].state =
                    ProcessState::READY;

                readyQueue.push(running);

                running = -1;
            }
        }

        // Select next process
        if (running == -1)
        {
            if (readyQueue.empty())
            {
                cout << "[t=" << currentTime
                     << "] CPU Idle" << endl;

                sleepOneUnit();

                currentTime++;

                continue;
            }

            running = readyQueue.top();
            readyQueue.pop();

            processes[running].state =
                ProcessState::RUNNING;

            if (processes[running].firstStart == -1)
            {
                processes[running].firstStart =
                    currentTime;
            }

            cout << "[t=" << currentTime
                 << "] Running S"
                 << processes[running].schedulerPid
                 << " | Priority = "
                 << processes[running].priority
                 << endl;
        }

        // Execute for one CPU unit
        sleepOneUnit();

        processes[running].remainingTime--;

        currentTime++;

        if (processes[running].remainingTime == 0)
        {
            processes[running].completion =
                currentTime;

            processes[running].state =
                ProcessState::TERMINATED;

            cout << "[t=" << currentTime
                 << "] S"
                 << processes[running].schedulerPid
                 << " completed."
                 << endl;

            completed++;

            running = -1;
        }
    }

    displayMetrics();
}

void Scheduler::displayMetrics() const
{
    double totalWaiting = 0;
    double totalTurnaround = 0;
    double totalResponse = 0;

    cout << "\nPerformance Metrics\n";
    cout << "-------------------------------------------------------------\n";

    cout << left
         << setw(8) << "PID"
         << setw(10) << "Burst"
         << setw(10) << "Arrival"
         << setw(12) << "Waiting"
         << setw(14) << "Turnaround"
         << setw(12) << "Response"
         << endl;

    for (const PCB &process : processes)
    {
        int turnaround =
            process.completion -
            process.arrivalTime;

        int waiting =
            turnaround -
            process.burstTime;

        int response =
            process.firstStart -
            process.arrivalTime;

        totalWaiting += waiting;
        totalTurnaround += turnaround;
        totalResponse += response;

        cout << left
             << setw(8)
             << ("S" + to_string(process.schedulerPid))

             << setw(10)
             << process.burstTime

             << setw(10)
             << process.arrivalTime

             << setw(12)
             << waiting

             << setw(14)
             << turnaround

             << setw(12)
             << response

             << endl;
    }

    double count =
        static_cast<double>(processes.size());

    cout << fixed << setprecision(2);

    cout << "\nAverage Waiting Time: "
         << totalWaiting / count
         << endl;

    cout << "Average Turnaround Time: "
         << totalTurnaround / count
         << endl;

    cout << "Average Response Time: "
         << totalResponse / count
         << endl;
}

// ============================================================
// Display all processes currently stored in the scheduler
// ============================================================

void Scheduler::showProcesses() const
{
    if (processes.empty())
    {
        cout << "No scheduled processes." << endl;
        return;
    }

    cout << "\nScheduled Processes" << endl;
    cout << "-------------------------------------------------------------" << endl;

    cout << left
         << setw(8)  << "PID"
         << setw(20) << "Command"
         << setw(10) << "Arrival"
         << setw(10) << "Burst"
         << setw(12) << "Remaining"
         << setw(10) << "Priority"
         << setw(15) << "State"
         << endl;

    cout << "-------------------------------------------------------------" << endl;

    for (const PCB &process : processes)
    {
        string stateName;

        switch (process.state)
        {
            case ProcessState::NEW:
                stateName = "NEW";
                break;

            case ProcessState::READY:
                stateName = "READY";
                break;

            case ProcessState::RUNNING:
                stateName = "RUNNING";
                break;

            case ProcessState::TERMINATED:
                stateName = "TERMINATED";
                break;
        }

        cout << left
             << setw(8)
             << ("S" + to_string(process.schedulerPid))

             << setw(20)
             << process.command

             << setw(10)
             << process.arrivalTime

             << setw(10)
             << process.burstTime

             << setw(12)
             << process.remainingTime

             << setw(10)
             << process.priority

             << setw(15)
             << stateName

             << endl;
    }
}


// ============================================================
// Run the currently selected scheduling algorithm
// ============================================================

void Scheduler::run()
{
    if (processes.empty())
    {
        cout << "No processes available to schedule." << endl;
        return;
    }

    // Reset process execution information before running
    for (PCB &process : processes)
    {
        process.remainingTime = process.burstTime;
        process.firstStart = -1;
        process.completion = -1;
        process.state = ProcessState::NEW;
    }

    if (algorithm == Algorithm::ROUND_ROBIN)
    {
        runRoundRobin();
    }
    else if (algorithm == Algorithm::PRIORITY)
    {
        runPriority();
    }
}


// ============================================================
// Display previously calculated performance metrics
// ============================================================

void Scheduler::showMetrics() const
{
    if (processes.empty())
    {
        cout << "No scheduled processes." << endl;
        return;
    }

    bool completedProcessesExist = false;

    for (const PCB &process : processes)
    {
        if (process.completion >= 0 &&
            process.firstStart >= 0)
        {
            completedProcessesExist = true;
            break;
        }
    }

    if (!completedProcessesExist)
    {
        cout << "No scheduling metrics available." << endl;
        cout << "Run the scheduler first using: run" << endl;
        return;
    }

    displayMetrics();
}


// ============================================================
// Clear scheduler processes and previous results
// ============================================================

void Scheduler::clear()
{
    processes.clear();

    nextPid = 1;
    nextSequence = 0;

    cout << "Scheduler queue and metrics cleared." << endl;
}