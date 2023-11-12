CC = gcc
CFLAGS = -Wall -Wextra
TARGET = L1Cache

# List of source files
SRCS = L1Cache.c SimpleProgram.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
.DEFAULT_GOAL := all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: run
run: $(TARGET)
	./$(TARGET)
