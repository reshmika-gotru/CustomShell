# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -std=c++17 -Wall

# Source Files
SRC = main.cpp shell.cpp parser.cpp builtin.cpp process.cpp

# Output File
TARGET = shell

# Build Target
all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Clean Compiled Files
clean:
	rm -f $(TARGET)