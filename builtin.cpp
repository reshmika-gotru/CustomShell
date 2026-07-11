#include "builtin.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <signal.h>

using namespace std;
namespace fs = filesystem;

bool executeBuiltin(vector<string> &args)
{
    if (args.empty())
        return true;

    string command = args[0];

    // pwd
    if (command == "pwd")
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        cout << cwd << endl;
        return true;
    }

    // cd
    else if (command == "cd")
    {
        if (args.size() < 2)
        {
            cout << "Usage: cd <directory>" << endl;
        }
        else
        {
            if (chdir(args[1].c_str()) != 0)
            {
                cout << "Directory not found." << endl;
            }
        }
        return true;
    }

    // echo
    else if (command == "echo")
    {
        for (size_t i = 1; i < args.size(); i++)
        {
            cout << args[i] << " ";
        }

        cout << endl;
        return true;
    }

    // clear
    else if (command == "clear")
    {
        system("clear");
        return true;
    }

    // ls
    else if (command == "ls")
    {
        system("ls");
        return true;
    }

    // mkdir
    else if (command == "mkdir")
    {
        if (args.size() < 2)
        {
            cout << "Usage: mkdir <directory>" << endl;
        }
        else
        {
            fs::create_directory(args[1]);
        }

        return true;
    }

    // rmdir
    else if (command == "rmdir")
    {
        if (args.size() < 2)
        {
            cout << "Usage: rmdir <directory>" << endl;
        }
        else
        {
            fs::remove(args[1]);
        }

        return true;
    }

    // rm
    else if (command == "rm")
    {
        if (args.size() < 2)
        {
            cout << "Usage: rm <filename>" << endl;
        }
        else
        {
            remove(args[1].c_str());
        }

        return true;
    }

    // touch
    else if (command == "touch")
    {
        if (args.size() < 2)
        {
            cout << "Usage: touch <filename>" << endl;
        }
        else
        {
            ofstream file(args[1]);
            file.close();
        }

        return true;
    }

    // cat
    else if (command == "cat")
    {
        if (args.size() < 2)
        {
            cout << "Usage: cat <filename>" << endl;
        }
        else
        {
            ifstream file(args[1]);

            if (!file)
            {
                cout << "File not found." << endl;
                return true;
            }

            string line;

            while (getline(file, line))
            {
                cout << line << endl;
            }

            file.close();
        }

        return true;
    }

    // kill
    else if (command == "kill")
    {
        if (args.size() < 2)
        {
            cout << "Usage: kill <pid>" << endl;
        }
        else
        {
            int pid = stoi(args[1]);

            if (::kill(pid, SIGTERM) == 0)
            {
                cout << "Process terminated." << endl;
            }
            else
            {
                cout << "Invalid PID." << endl;
            }
        }

        return true;
    }

    // Not a built-in command
    return false;
}