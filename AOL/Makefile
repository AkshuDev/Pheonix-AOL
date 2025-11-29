# === Compiler Settings ===
CXX := clang++
CXXFLAGS := -std=c++20 -Wall -Wextra -Iinclude

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

all: build $(TARGET) gen-nfx
	@echo "Build complete ("$(MODE)") -> $(TARGET)"

build:
	@mkdir -p build
	@mkdir -p dist
	@mkdir -p dist/aol

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate NFX metadata file
gen-nfx:
	@rm -rf dist/aol/nfx.json
	@echo "Generating nfx.json..."
	@echo '{'                                                        >> dist/aol/nfx.json
	@echo '	"Name": "aol",'                                          >> dist/aol/nfx.json
	@echo '	"Binaries": ['                                           >> dist/aol/nfx.json
	@echo '		{'                                                   >> dist/aol/nfx.json
	@echo '			"Os": ["linux"],'                                >> dist/aol/nfx.json
	@echo '			"Arch": ["x86_64"],'                             >> dist/aol/nfx.json
	@echo '			"Path": "aol_linux_x86_64",'                     >> dist/aol/nfx.json
	@echo '			"Name": "aol"'                                   >> dist/aol/nfx.json
	@echo '		},'                                                  >> dist/aol/nfx.json
	@echo '		{'                                                   >> dist/aol/nfx.json
	@echo '			"Os": ["windows"],'                              >> dist/aol/nfx.json
	@echo '			"Arch": ["x86_64"],'                             >> dist/aol/nfx.json
	@echo '			"Path": "aol_windows_x86_64.exe",'               >> dist/aol/nfx.json
	@echo '			"Name": "aol"'                                   >> dist/aol/nfx.json
	@echo '		},'                                                  >> dist/aol/nfx.json
	@echo '		{'                                                   >> dist/aol/nfx.json
	@echo '			"Os": ["macos"],'                                >> dist/aol/nfx.json
	@echo '			"Arch": ["arm64"],'                              >> dist/aol/nfx.json
	@echo '			"Path": "aol_macos_arm64",'                      >> dist/aol/nfx.json
	@echo '			"Name": "aol"'                                   >> dist/aol/nfx.json
	@echo '		}'                                                   >> dist/aol/nfx.json
	@echo '	]'                                                       >> dist/aol/nfx.json
	@echo '}'                                                        >> dist/aol/nfx.json

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
