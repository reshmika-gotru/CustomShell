#ifndef BUILTIN_H
#define BUILTIN_H

#include <vector>
#include <string>

using namespace std;

// Executes built-in commands
// Returns true if the command is built-in, otherwise false.
bool executeBuiltin(vector<string> &args);

#endif