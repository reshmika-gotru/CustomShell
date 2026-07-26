# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -std=c++17 -Wall

# Source Files
SRC = main.cpp shell.cpp parser.cpp builtin.cpp process.cpp

# Output File
TARGET = shell

SOURCES = main.cpp shell.cpp parser.cpp builtin.cpp process.cpp scheduler.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET) ./$(TARGET)