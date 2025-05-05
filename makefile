# Makefile for OS Simulator Project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I./include

# Directory paths
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/os_simulator

# Source and object files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Header files
HEADERS = $(wildcard include/*.h)

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Link object files to create final binary
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Build successful! Run with: $(TARGET)"

# Compile .c files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the simulator
run: $(TARGET)
	./$(TARGET)

# Memory leak check
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Create a sample synthetic program file
test_program:
	@echo "Creating test program..."
	@mkdir -p test
	@echo "TestProcess" > test/program.txt
	@echo "1" >> test/program.txt
	@echo "10" >> test/program.txt
	@echo "4" >> test/program.txt
	@echo "sem1 sem2" >> test/program.txt
	@echo "exec 50" >> test/program.txt
	@echo "P(sem1)" >> test/program.txt
	@echo "read 5" >> test/program.txt
	@echo "write 3" >> test/program.txt
	@echo "V(sem1)" >> test/program.txt
	@echo "exec 20" >> test/program.txt
	@echo "Test program created at test/program.txt"

.PHONY: all clean run memcheck test_program directories
