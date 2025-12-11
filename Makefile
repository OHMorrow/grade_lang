# Simple Makefile for GradeLang project

# Compiler and flags
CXX ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall -I.
LDFLAGS ?=

# Detect platform-specific executable suffix
ifeq ($(OS),Windows_NT)
    EXE := gradelang.exe
else
    EXE := gradelang
endif

# Source and object files (automatic)
SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)

# Default target
all: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Generic rule to build .o from .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean run rebuild

clean:
	$(RM) $(OBJECTS) $(EXE)

rebuild: clean all

# run target invokes the program with optional args passed via ARGS
run: all
	./$(EXE) $(ARGS)

#Helpful print of variables
print:
	@echo "CXX=$(CXX)"
	@echo "CXXFLAGS=$(CXXFLAGS)"
	@echo "SOURCES=$(SOURCES)"
	@echo "OBJECTS=$(OBJECTS)"
	@echo "EXE=$(EXE)"
