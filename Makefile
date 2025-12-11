# Makefile for GradeLang
# Builds main.cpp and all .cpp files in src/, using headers from include/

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Iinclude -Wall -Wextra -g

SRC_DIR := src

# new: object files coming only from src/ (exclude main.o)
SRC_OBJS := $(patsubst %.cpp,%.o,$(wildcard $(SRC_DIR)/*.cpp))
# new: print_ast needs print_ast.o + src object files
PRINT_OBJS := print_ast.o $(SRC_OBJS)
# new: tests executable needs test/tests.o + src object files
TEST_OBJS := test/tests.o $(SRC_OBJS)
OBJS := main.o $(SRC_OBJS)
# include test objects so clean removes them
ALL_OBJS := $(OBJS) $(PRINT_OBJS) $(TEST_OBJS)

TARGET := gradelang

# include tests in targets list
ALL_TARGETS := $(TARGET) print_ast tests

.PHONY: all clean run debug print_ast tests

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# new target to produce print_ast executable
print_ast: $(PRINT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(PRINT_OBJS)

# new target to produce tests executable
tests: $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(ALL_OBJS) $(ALL_TARGETS)

debug: CXXFLAGS += -O0 -ggdb
debug: clean all

run: all
	./$(TARGET)
