#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

using namespace std;

// Parses the user input into command and arguments
vector<string> parseCommand(string input, bool &background);

#endif