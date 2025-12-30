# Makefile for building the Electron visualizer on Linux


SRCDIR := src
OBJDIR := build
TARGET := electron

CXX := g++
# Paths to your local SFML installation
# If you later install SFML to a system location, update or remove these
SFML_INCLUDE := /home/kali/SFML-2.6.2/include
SFML_LIBDIR := /home/kali/SFML-2.6.2/lib

# include directories: project src and SFML include
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -I$(SRCDIR) -I$(SFML_INCLUDE)
# linker flags: point to SFML lib dir and link the needed SFML libs
LDFLAGS := -Wl,-rpath,$(SFML_LIBDIR) -L$(SFML_LIBDIR) -lsfml-graphics -lsfml-window -lsfml-system

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

.PHONY: all clean dirs run

all: dirs $(OBJDIR)/$(TARGET)

dirs:
	mkdir -p $(OBJDIR) $(OBJDIR)

$(OBJDIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(OBJDIR)

run: all
	./$(OBJDIR)/$(TARGET)
