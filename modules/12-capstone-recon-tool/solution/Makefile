# Makefile for reconc -- the Module 12 capstone.
#
# Targets:
#   make            build the optimised binary ./reconc
#   make debug      rebuild with -O0 -g3 for gdb
#   make asan       rebuild with AddressSanitizer + UBSan for runtime checks
#   make run        build and run against localhost
#   make clean      remove build artifacts
#   make help       list these targets
#
# The build is deliberately simple: every .c in src/ compiles to a .o, and the
# .o files link into one binary. Header dependencies are tracked automatically
# (see DEPFLAGS below), so editing a .h rebuilds exactly the files that use it.

CC      ?= gcc

# Language + warnings. -std=gnu11 (not c11) because we use POSIX/GNU functions
# such as nftw(3), strdup(3) and getline(3); the code also #defines _GNU_SOURCE
# in each .c for the same reason.
CSTD    := -std=gnu11
WARN    := -Wall -Wextra
CFLAGS  ?= $(CSTD) $(WARN) -O2
LDFLAGS ?=

# Auto-generated header dependencies: -MMD writes a .d file per .o listing the
# headers it included; -MP adds phony targets so a deleted header isn't fatal.
DEPFLAGS := -MMD -MP

SRC_DIR := src
BIN     := reconc

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(SOURCES:.c=.o)
DEPS    := $(OBJECTS:.o=.d)

.PHONY: all clean debug asan run help

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

# Pattern rule: compile one source file to an object file.
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Debug build: no optimisation, full debug info for gdb. The target-specific
# CFLAGS assignment applies to this target and its prerequisites only.
debug: CFLAGS := $(CSTD) $(WARN) -O0 -g3
debug: clean $(BIN)

# AddressSanitizer + UndefinedBehaviorSanitizer. Catches leaks, use-after-free,
# buffer overruns and UB at runtime. Run ./reconc after building to see it work.
asan: CFLAGS  := $(CSTD) $(WARN) -O1 -g3 -fsanitize=address,undefined -fno-omit-frame-pointer
asan: LDFLAGS := -fsanitize=address,undefined
asan: clean $(BIN)

run: $(BIN)
	./$(BIN)

clean:
	$(RM) $(OBJECTS) $(DEPS) $(BIN)

help:
	@echo "targets: all (default), debug, asan, run, clean"

# Pull in the auto-generated dependency files if they exist. The leading '-'
# means "don't error on the first build when they don't yet exist".
-include $(DEPS)
