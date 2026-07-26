#include "shell.h"
#include "parser.h"
#include "builtin.h"
#include "process.h"
#include "scheduler.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

void startShell()
{
    string input;
    vector<string> args;
    bool background = false;

    // Scheduler object used for Deliverable 2
    Scheduler scheduler;

    cout << "==================================" << endl;
    cout << "      Welcome to MyShell" << endl;
    cout << "Type 'exit' to quit the shell." << endl;
    cout << "==================================" << endl;

    while (true)
    {
        cout << "\nMyShell> ";

        if (!getline(cin, input))
        {
            cout << "\nExiting MyShell..." << endl;
            break;
        }

        if (input.empty())
        {
            continue;
        }

        // Use the same Deliverable 1 parser.
        args = parseCommand(input, background);

        if (args.empty())
        {
            continue;
        }

        string command = args[0];

        // =========================================================
        // EXIT COMMAND
        // =========================================================

        if (command == "exit")
        {
            cout << "Exiting MyShell..." << endl;
            break;
        }

        // =========================================================
        // DELIVERABLE 1 JOB CONTROL
        // =========================================================

        // Display background jobs
        if (command == "jobs")
        {
            showJobs();
            continue;
        }

        // Bring a background job to the foreground
        if (command == "fg")
        {
            if (args.size() < 2)
            {
                cout << "Usage: fg <jobID>" << endl;
            }
            else
            {
                try
                {
                    int jobID = stoi(args[1]);
                    bringToForeground(jobID);
                }
                catch (...)
                {
                    cout << "Invalid job ID." << endl;
                }
            }

            continue;
        }

        // Resume a stopped background job
        if (command == "bg")
        {
            if (args.size() < 2)
            {
                cout << "Usage: bg <jobID>" << endl;
            }
            else
            {
                try
                {
                    int jobID = stoi(args[1]);
                    resumeBackground(jobID);
                }
                catch (...)
                {
                    cout << "Invalid job ID." << endl;
                }
            }

            continue;
        }

        // =========================================================
        // DELIVERABLE 2: SCHEDULER CONFIGURATION
        // =========================================================

        /*
         * Round-Robin:
         *
         * schedule rr <quantum> [delay_ms]
         *
         * Example:
         * schedule rr 2 250
         *
         * Priority:
         *
         * schedule priority [delay_ms]
         *
         * Example:
         * schedule priority 250
         */

        if (command == "schedule")
        {
            if (args.size() < 2)
            {
                cout << "Usage:" << endl;
                cout << "  schedule rr <quantum> [delay_ms]" << endl;
                cout << "  schedule priority [delay_ms]" << endl;
                continue;
            }

            string algorithm = args[1];

            // -----------------------------------------------------
            // ROUND-ROBIN CONFIGURATION
            // -----------------------------------------------------

            if (algorithm == "rr")
            {
                if (args.size() < 3)
                {
                    cout << "Usage: schedule rr <quantum> [delay_ms]"
                         << endl;

                    continue;
                }

                try
                {
                    int quantum = stoi(args[2]);

                    int delayMs = 0;

                    if (args.size() >= 4)
                    {
                        delayMs = stoi(args[3]);
                    }

                    if (quantum <= 0)
                    {
                        cout << "Error: quantum must be greater than 0."
                             << endl;

                        continue;
                    }

                    if (delayMs < 0)
                    {
                        cout << "Error: delay cannot be negative."
                             << endl;

                        continue;
                    }

                    scheduler.setRoundRobin(
                        quantum,
                        delayMs
                    );
                }
                catch (...)
                {
                    cout << "Error: invalid Round-Robin configuration."
                         << endl;
                }

                continue;
            }

            // -----------------------------------------------------
            // PRIORITY CONFIGURATION
            // -----------------------------------------------------

            else if (algorithm == "priority")
            {
                try
                {
                    int delayMs = 0;

                    if (args.size() >= 3)
                    {
                        delayMs = stoi(args[2]);
                    }

                    if (delayMs < 0)
                    {
                        cout << "Error: delay cannot be negative."
                             << endl;

                        continue;
                    }

                    scheduler.setPriority(delayMs);
                }
                catch (...)
                {
                    cout << "Error: invalid Priority configuration."
                         << endl;
                }

                continue;
            }

            else
            {
                cout << "Unknown scheduling algorithm." << endl;
                cout << "Available algorithms:" << endl;
                cout << "  rr" << endl;
                cout << "  priority" << endl;

                continue;
            }
        }

        // =========================================================
        // DELIVERABLE 2: ADD PROCESS TO SCHEDULER
        // =========================================================

        /*
         * Syntax:
         *
         * add <burst> <priority> <arrival> <command...>
         *
         * Examples:
         *
         * add 5 2 0 task-A
         * add 3 1 0 task-B
         * add 4 3 1 task-C
         */

        if (command == "add")
        {
            if (args.size() < 5)
            {
                cout << "Usage: add <burst> <priority> "
                     << "<arrival> <command>"
                     << endl;

                continue;
            }

            try
            {
                int burst = stoi(args[1]);
                int priority = stoi(args[2]);
                int arrival = stoi(args[3]);

                // The process name/command may contain several words.
                string processCommand;

                for (size_t i = 4; i < args.size(); i++)
                {
                    if (!processCommand.empty())
                    {
                        processCommand += " ";
                    }

                    processCommand += args[i];
                }

                // Validate process information.
                if (burst <= 0)
                {
                    cout << "Error: burst time must be greater than 0."
                         << endl;

                    continue;
                }

                if (priority < 0)
                {
                    cout << "Error: priority cannot be negative."
                         << endl;

                    continue;
                }

                if (arrival < 0)
                {
                    cout << "Error: arrival time cannot be negative."
                         << endl;

                    continue;
                }

                if (processCommand.empty())
                {
                    cout << "Error: process command cannot be empty."
                         << endl;

                    continue;
                }

                scheduler.addProcess(
                    burst,
                    priority,
                    arrival,
                    processCommand
                );
            }
            catch (...)
            {
                cout << "Error: invalid process information."
                     << endl;

                cout << "Usage: add <burst> <priority> "
                     << "<arrival> <command>"
                     << endl;
            }

            continue;
        }

        // =========================================================
        // DISPLAY SCHEDULED PROCESSES
        // =========================================================

        if (command == "schedjobs")
        {
            scheduler.showProcesses();
            continue;
        }

        // =========================================================
        // RUN SELECTED SCHEDULING ALGORITHM
        // =========================================================

        if (command == "run")
        {
            scheduler.run();
            continue;
        }

        // =========================================================
        // DISPLAY PERFORMANCE METRICS
        // =========================================================

        if (command == "metrics")
        {
            scheduler.showMetrics();
            continue;
        }

        // =========================================================
        // CLEAR SCHEDULER
        // =========================================================

        if (command == "schedclear")
        {
            scheduler.clear();
            continue;
        }

        // =========================================================
        // SCHEDULER HELP
        // =========================================================

        if (command == "schedhelp")
        {
            cout << "\nScheduling Commands" << endl;
            cout << "============================================"
                 << endl;

            cout << "schedule rr <quantum> [delay_ms]"
                 << endl;

            cout << "    Configure Round-Robin scheduling."
                 << endl;

            cout << endl;

            cout << "schedule priority [delay_ms]"
                 << endl;

            cout << "    Configure Preemptive Priority scheduling."
                 << endl;

            cout << endl;

            cout << "add <burst> <priority> <arrival> <command>"
                 << endl;

            cout << "    Add a simulated process."
                 << endl;

            cout << endl;

            cout << "schedjobs"
                 << endl;

            cout << "    Display scheduled processes."
                 << endl;

            cout << endl;

            cout << "run"
                 << endl;

            cout << "    Run the selected scheduling algorithm."
                 << endl;

            cout << endl;

            cout << "metrics"
                 << endl;

            cout << "    Display scheduling performance metrics."
                 << endl;

            cout << endl;

            cout << "schedclear"
                 << endl;

            cout << "    Clear scheduled processes and metrics."
                 << endl;

            cout << endl;

            cout << "schedhelp"
                 << endl;

            cout << "    Display scheduler commands."
                 << endl;

            cout << "============================================"
                 << endl;

            continue;
        }

        // =========================================================
        // ORIGINAL DELIVERABLE 1 BUILT-IN COMMANDS
        // =========================================================

        /*
         * The original Deliverable 1 built-in mechanism remains
         * unchanged.
         *
         * Examples:
         *
         * pwd
         * cd
         * echo
         * clear
         * ls
         * mkdir
         * rmdir
         * rm
         * touch
         * cat
         * kill
         */

        if (executeBuiltin(args))
        {
            continue;
        }

        // =========================================================
        // ORIGINAL DELIVERABLE 1 EXTERNAL PROCESS EXECUTION
        // =========================================================

        /*
         * Commands that are not shell built-ins and not scheduling
         * commands continue to use the original executeProcess()
         * implementation.
         *
         * This preserves:
         *
         * fork()
         * execvp()
         * waitpid()
         * foreground execution
         * background execution using &
         */

        executeProcess(args, background);
    }
}