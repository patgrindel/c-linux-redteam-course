# Annotated Makefile for the stats project (Module 11).
# Build:  make        Clean:  make clean        Run:  ./stats 3 1 4

# ---- Variables -------------------------------------------------------------
CC      := gcc                       # the compiler
CFLAGS  := -std=c11 -Wall -Wextra -g # flags for every compile; -g adds debug info
TARGET  := stats                     # the program we want to produce
OBJS    := main.o stats.o            # one .o per .c

# ---- Default target --------------------------------------------------------
# The FIRST target is what `make` with no arguments builds. .PHONY targets
# aren't files; declare them so make never skips them because a file matches.
.PHONY: all clean
all: $(TARGET)

# ---- Link step -------------------------------------------------------------
# $(TARGET) depends on all object files; if any is newer, relink.
# $@ = the target name, $^ = all prerequisites.
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ---- Compile step (pattern rule) -------------------------------------------
# "%.o from the matching %.c" plus the header. $< = the first prerequisite.
# Listing stats.h means: if the header changes, dependent .o files rebuild.
%.o: %.c stats.h
	$(CC) $(CFLAGS) -c -o $@ $<

# ---- Housekeeping ----------------------------------------------------------
clean:
	rm -f $(TARGET) $(OBJS)
