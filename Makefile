# Directories
INCLUDE_DIR 	:= include
SRC_DIR 		:= src
OBJ_DIR 		:= obj
BIN_DIR 		:= bin

# Compiler and flags
CC 				:= gcc
CFLAGS 			:= -std=gnu11 -Wall -Wextra -O2 -I$(INCLUDE_DIR)
LDFLAGS 		:= -lm

# Files
SRC 			:= $(wildcard $(SRC_DIR)/*.c)
OBJ 			:= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
TARGET 			:= $(BIN_DIR)/bthread_demo

# Default target
all: $(BIN_DIR) $(OBJ_DIR) $(TARGET)

# Build binary
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create bin and obj directories if they don't exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean

