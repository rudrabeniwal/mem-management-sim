CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./include

# Sources
SRCS = src/main.cpp src/MemoryManager.cpp src/cache/Cache.cpp src/virtual_memory/PageTable.cpp src/allocator/Allocator.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = memsim

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
