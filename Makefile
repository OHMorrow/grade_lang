# Makefile for GradeLang
# Builds main.cpp and all .cpp files in src/, using headers from include/

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Iinclude -Wall -Wextra -g

SRC_DIR := src
SRCS := main.cpp $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:.cpp=.o)

TARGET := gradelang

.PHONY: all clean run debug

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

debug: CXXFLAGS += -O0 -ggdb
debug: clean all

run: all
	./$(TARGET)
