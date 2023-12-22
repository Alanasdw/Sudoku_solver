.PHONY: all clean

CC = gcc
CFLAGS = -Wall -g -O3 -march=native -fopenmp

SOURCE = structured_stack_omp.c # naive.c naive_stack.c structured_stack.c
TARGET = $(SOURCE:.c=)

all: $(TARGET)

$(TARGET) : % : %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(TARGET)
