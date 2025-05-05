# Makefile for OS Simulator Project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I./include

# Source files and object files
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Collect all source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
TARGET = $(BIN_DIR)/os_simulator

# Header files
HEADERS = $(wildcard include/*.h)

# Rule to create directories if they don't exist
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# Default target
all: $(TARGET)

# Rule to link object files and create the executable
$(TARGET): $(OBJECTS)
    $(CC) $(CFLAGS) $^ -o $@
    @echo "Build successful! Run with: $(TARGET)"

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
    $(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
    rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the simulator
run: $(TARGET)
    ./$(TARGET)

# Debug with valgrind to check for memory leaks
memcheck: $(TARGET)
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Create test program
test_program:
    @echo "Creating test program..."
    @mkdir -p test
    @echo "TestProcess" > test/program.txt
    @echo "1" >> test/program.txt
    @echo "10" >> test/program.txt
    @echo "4" >> test/program.txt
    @echo "sem1 sem2" >> test/program.txt
    @echo "exec 50" >> test/program.txt
    @echo "P $sem1" >> test/program.txt
    @echo "read 5" >> test/program.txt
    @echo "write 3" >> test/program.txt
    @echo "V $sem1" >> test/program.txt
    @echo "exec 20" >> test/program.txt
    @echo "Test program created at test/program.txt"

.PHONY: all clean run memcheck test_program