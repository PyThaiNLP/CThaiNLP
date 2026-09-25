# Makefile for CThaiNLP

CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
AR = ar
ARFLAGS = rcs

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
EXAMPLES_DIR = examples
LIB_DIR = lib

# Source files
SOURCES = $(SRC_DIR)/trie.c $(SRC_DIR)/tcc.c $(SRC_DIR)/newmm.c $(SRC_DIR)/util.c $(SRC_DIR)/soundex.c
OBJECTS = $(BUILD_DIR)/trie.o $(BUILD_DIR)/tcc.o $(BUILD_DIR)/newmm.o $(BUILD_DIR)/util.o $(BUILD_DIR)/soundex.o

# Library
LIBRARY = $(LIB_DIR)/libcthainlp.a

# Example programs
EXAMPLE_BASIC = $(BUILD_DIR)/example_basic
COUNT_WORDS = $(BUILD_DIR)/count_words
EXAMPLES = $(EXAMPLE_BASIC) $(COUNT_WORDS)

# Test programs
TEST_NEWMM = $(BUILD_DIR)/test_newmm
TEST_TCC = $(BUILD_DIR)/test_tcc
TEST_UTIL = $(BUILD_DIR)/test_util
TEST_SOUNDEX = $(BUILD_DIR)/test_soundex
ALL_TESTS = $(TEST_NEWMM) $(TEST_TCC) $(TEST_UTIL) $(TEST_SOUNDEX)

# Default target
all: dirs $(LIBRARY) $(EXAMPLES) $(ALL_TESTS)

# Create directories
dirs:
	@mkdir -p $(BUILD_DIR) $(LIB_DIR)

# Build object files
$(BUILD_DIR)/trie.o: $(SRC_DIR)/trie.c $(SRC_DIR)/trie.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tcc.o: $(SRC_DIR)/tcc.c $(SRC_DIR)/tcc.h $(INCLUDE_DIR)/tcc.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/newmm.o: $(SRC_DIR)/newmm.c $(SRC_DIR)/trie.h $(SRC_DIR)/tcc.h $(INCLUDE_DIR)/newmm.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/util.o: $(SRC_DIR)/util.c $(INCLUDE_DIR)/util.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/soundex.o: $(SRC_DIR)/soundex.c $(INCLUDE_DIR)/soundex.h $(INCLUDE_DIR)/util.h | dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Build library
$(LIBRARY): $(OBJECTS) | dirs
	$(AR) $(ARFLAGS) $@ $^

# Build example programs
$(EXAMPLE_BASIC): $(EXAMPLES_DIR)/example_basic.c $(LIBRARY) | dirs
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lcthainlp -o $@

$(COUNT_WORDS): $(EXAMPLES_DIR)/count_words.c $(LIBRARY) | dirs
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lcthainlp -o $@

# Build test programs
$(TEST_NEWMM): tests/test_newmm.c $(LIBRARY) | dirs
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lcthainlp -o $@

$(TEST_TCC): tests/test_tcc.c $(LIBRARY) | dirs
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lcthainlp -o $@

$(TEST_UTIL): tests/test_util.c $(LIBRARY) | dirs
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lcthainlp -o $@

$(TEST_SOUNDEX): tests/test_soundex.c $(LIBRARY) | dirs
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lcthainlp -o $@

# Test target
test: dirs $(ALL_TESTS)
	./$(TEST_NEWMM)
	./$(TEST_TCC)
	./$(TEST_UTIL)
	./$(TEST_SOUNDEX)

# Benchmark target
benchmark:
	python3 examples/python/benchmark.py --report BENCHMARK.md

benchmark-quick:
	python3 examples/python/benchmark.py --quick

# Clean
clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR)

.PHONY: all dirs clean test benchmark benchmark-quick
