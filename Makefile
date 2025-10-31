# Directories
INCLUDE_DIR 	:= include
SRC_DIR 		:= src
OBJ_DIR 		:= obj
BIN_DIR 		:= bin
TEST_DIR 		:= tests

# Compiler and flags
CC 				:= gcc
CFLAGS 			:= -std=gnu11 -Wall -Wextra -O2 -I$(INCLUDE_DIR)
LDFLAGS 		:= -lm

# Files
SRC 			:= $(wildcard $(SRC_DIR)/*.c)
OBJ 			:= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
TARGET 			:= $(BIN_DIR)/bthread_demo

# Test files
TEST_SRC 		:= $(wildcard $(TEST_DIR)/*_test.c)
TEST_OBJ 		:= $(filter-out $(OBJ_DIR)/bthread_demo.o, $(OBJ))
TEST_BINS 		:= $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%,$(TEST_SRC))

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

# Build test binaries
$(BIN_DIR)/%_test: $(TEST_DIR)/%_test.c $(TEST_OBJ)
	$(CC) $(CFLAGS) $< $(TEST_OBJ) -o $@ $(LDFLAGS)

# Run all tests
test: $(BIN_DIR) $(OBJ_DIR) $(TEST_BINS)
	@echo "Running all tests..."
	@for test in $(TEST_BINS); do \
		echo ""; \
		$$test || exit 1; \
	done
	@echo ""
	@echo "All tests passed!"

# Clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean test

