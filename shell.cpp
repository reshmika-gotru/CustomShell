#include "shell.h"
#include "parser.h"
#include "builtin.h"
#include "process.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void startShell()
{
    string input;
    vector<string> args;
    bool background = false;

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

        args = parseCommand(input, background);

        if (args.empty())
        {
            continue;
        }

        string command = args[0];

        // Exit command
        if (command == "exit")
        {
            cout << "Exiting MyShell..." << endl;
            break;
        }

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

        // Execute built-in commands
        if (executeBuiltin(args))
        {
            continue;
        }

        // Execute external commands
        executeProcess(args, background);
    }
}