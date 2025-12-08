CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./include
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = electron

SRCS = src/main.cpp src/electron.cpp src/utils.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean run