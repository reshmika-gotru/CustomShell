#include "process.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstring>

using namespace std;

// Stores all background jobs
vector<Job> jobs;

int nextJobID = 1;

void executeProcess(vector<string> &args, bool background)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        cout << "Error creating process." << endl;
        return;
    }

    // Child Process
    if (pid == 0)
    {
        vector<char*> command;

        for (string &arg : args)
        {
            command.push_back(const_cast<char*>(arg.c_str()));
        }

        command.push_back(nullptr);

        execvp(command[0], command.data());

        cout << "Invalid command." << endl;
        exit(EXIT_FAILURE);
    }

    // Parent Process
    else
    {
        if (background)
        {
            Job job;

            job.jobID = nextJobID++;
            job.pid = pid;
            job.command = args[0];
            job.running = true;

            jobs.push_back(job);

            cout << "[" << job.jobID << "] "
                 << "Process started with PID "
                 << pid << endl;
        }
        else
        {
            waitpid(pid, nullptr, 0);
        }
    }
}

// Display background jobs
void showJobs()
{
    cout << "\nBackground Jobs\n";
    cout << "--------------------------\n";

    for (auto &job : jobs)
    {
        cout << "[" << job.jobID << "] "
             << "PID: " << job.pid
             << " Command: " << job.command
             << " Status: ";

        if (job.running)
            cout << "Running";
        else
            cout << "Stopped";

        cout << endl;
    }
}

// Bring job to foreground
void bringToForeground(int jobID)
{
    for (auto &job : jobs)
    {
        if (job.jobID == jobID)
        {
            cout << "Foreground Process: "
                 << job.command << endl;

            waitpid(job.pid, nullptr, 0);

            job.running = false;

            return;
        }
    }

    cout << "Job not found." << endl;
}


// Resume stopped background job
void resumeBackground(int jobID)
{
    for (auto &job : jobs)
    {
        if (job.jobID == jobID)
        {
            kill(job.pid, SIGCONT);

            job.running = true;

            cout << "Job "
                 << job.jobID
                 << " resumed." << endl;

            return;
        }
    }

    cout << "Job not found." << endl;
}