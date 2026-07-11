#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <string>
#include <sys/types.h>

using namespace std;

// Structure to store background job information
struct Job
{
    int jobID;
    pid_t pid;
    string command;
    bool running;
};

// Executes external commands
void executeProcess(vector<string> &args, bool background);

// Display all background jobs
void showJobs();

// Bring a background job to foreground
void bringToForeground(int jobID);

// Resume a stopped job in the background
void resumeBackground(int jobID);

#endif