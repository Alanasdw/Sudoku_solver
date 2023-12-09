.PHONY: all clean

CC = gcc
CFLAGS = -Wall -g -O3 -march=native

SOURCE = naive.c naive_stack.c
TARGET = $(SOURCE:.c=)

all: $(TARGET)

$(TARGET) : % : %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(TARGET)
