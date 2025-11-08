# Directories
INCLUDE_DIR 	:= include
SRC_DIR 		:= src
OBJ_DIR 		:= obj
BIN_DIR 		:= bin
TEST_DIR 		:= tests
LIB_DIR 		:= lib

# Detect platform
UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
	PLATFORM := Windows
	SHARED_EXT := .dll
	SHARED_FLAGS := -shared
	LIB_PREFIX :=
else ifeq ($(UNAME_S),Linux)
	PLATFORM := Linux
	SHARED_EXT := .so
	SHARED_FLAGS := -shared -fPIC
	LIB_PREFIX := lib
else ifeq ($(UNAME_S),Darwin)
	PLATFORM := macOS
	SHARED_EXT := .dylib
	SHARED_FLAGS := -dynamiclib -fPIC
	LIB_PREFIX := lib
else
	PLATFORM := Unknown
	SHARED_EXT := .so
	SHARED_FLAGS := -shared -fPIC
	LIB_PREFIX := lib
endif

# Compiler and flags
CC 				:= gcc
CFLAGS 			:= -std=gnu11 -Wall -Wextra -O2 -I$(INCLUDE_DIR)
SHARED_CFLAGS 	:= $(CFLAGS) -fPIC
LDFLAGS 		:= -lm

# Files
DEMO_DIR 		:= demos
SRC 			:= $(wildcard $(SRC_DIR)/*.c)
OBJ 			:= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
SHARED_OBJ 		:= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.shared.o,$(SRC))
TARGET 			:= $(BIN_DIR)/bthread_demo
LIB_NAME 		:= bthread
SHARED_LIB 		:= $(LIB_DIR)/$(LIB_PREFIX)$(LIB_NAME)$(SHARED_EXT)

# Test files
TEST_SRC 		:= $(wildcard $(TEST_DIR)/*_test.c)
TEST_OBJ 		:= $(filter-out $(OBJ_DIR)/bthread_demo.o, $(OBJ))
TEST_BINS 		:= $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%,$(TEST_SRC))

# Default target
all: $(BIN_DIR) $(OBJ_DIR) $(TARGET) $(SHARED_LIB)

# Build shared library
lib: $(OBJ_DIR) $(SHARED_LIB)

$(SHARED_LIB): $(SHARED_OBJ)
	@mkdir -p $(LIB_DIR)
	@echo "Building shared library for $(PLATFORM)..."
	$(CC) $(SHARED_FLAGS) $(SHARED_OBJ) -o $@ $(LDFLAGS)

# Build binary
$(TARGET): $(DEMO_DIR)/bthread_demo.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build shared object files
$(OBJ_DIR)/%.shared.o: $(SRC_DIR)/%.c
	$(CC) $(SHARED_CFLAGS) -c $< -o $@

# Create bin, obj, and lib directories if they don't exist
$(BIN_DIR) $(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

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
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

.PHONY: all clean test lib

