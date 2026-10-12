# ex1.c equivalent: a Makefile that builds greeter.c + ex1_main.c -> greet.
# Copy greeter.h, greeter.c, ex1_main.c from the exercises folder next to this
# Makefile, then run `make`.

CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -g
TARGET  := greet
OBJS    := ex1_main.o greeter.o

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c greeter.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)
