# === Compiler Settings ===
CXX := clang++
CXXFLAGS := -std=c++20 -Wall -Wextra -Iinclude -fsanitize=address -fno-omit-frame-pointer

# Build modes
DEBUG_FLAGS := -g -O0 -I inc
RELEASE_FLAGS := -O3

# Detect platform
OS := $(shell uname -s | tr '[:upper:]' '[:lower:]')
ARCH := $(shell uname -m)

# Normalize arch names
ifeq ($(ARCH),x86_64)
    ARCH := x86_64
endif
ifeq ($(ARCH),amd64)
    ARCH := x86_64
endif

# Output binary format
TARGET := dist/aol/aol_$(OS)_$(ARCH)

# Find all source files
SRC := $(wildcard src/*.cpp)
OBJ := $(patsubst src/%.cpp, build/%.o, $(SRC))

# Default build mode
MODE ?= debug

ifeq ($(MODE),release)
    CXXFLAGS += $(RELEASE_FLAGS)
else
    CXXFLAGS += $(DEBUG_FLAGS)
endif

# === Rules ===

all: build $(TARGET)
	@echo "Build complete ("$(MODE)") -> $(TARGET)"

build:
	@mkdir -p build
	@mkdir -p dist
	@mkdir -p dist/aol

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) -fsanitize=address

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build dist

rebuild: clean all

run: all
	./$(TARGET)

install: $(TARGET)
	@echo "Installing via NFX..."
	nfx genconfig install:dist/aol:local

help:
	@echo "Available commands:"
	@echo "  make              - Build (debug default)"
	@echo "  make MODE=release - Optimized release build"
	@echo "  make run          - Build then run"
	@echo "  make clean        - Remove build files"
	@echo "  make rebuild      - Full clean + rebuild"
	@echo "  make install      - Install binary system-wide with NFX"
