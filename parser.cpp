#include "parser.h"

#include <sstream>

using namespace std;

vector<string> parseCommand(string input, bool &background)
{
    vector<string> args;
    string word;

    background = false;

    stringstream ss(input);

    // Split input into words
    while (ss >> word)
    {
        args.push_back(word);
    }

    // Check if the last argument is "&"
    if (!args.empty() && args.back() == "&")
    {
        background = true;
        args.pop_back();   // Remove "&" from the arguments
    }

    return args;
}