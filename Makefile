# Makefile to compile main.exe in the out folder (Windows-friendly)

# Compiler and flags
CXX      := clang++
CXXFLAGS := -O3 -std=c++17
LDFLAGS  := -lkernel32 -luser32 -lgdi32

# Directories and files
OUT_DIR  := out
TARGET   := $(OUT_DIR)\main.exe
SRCS     := main.cpp Snake.cpp
HEADERS  := Snake.h

# Default target: clean then build
all: clean $(TARGET)

# Build rule
$(TARGET): $(SRCS)
	@if not exist $(OUT_DIR) mkdir $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Clean target
clean:
	@if exist $(OUT_DIR) del /Q $(OUT_DIR)\*.exe 2>nul

# Run target
run: all
	@.\$(TARGET)
