.PHONY: all clean

CC = gcc
CFLAGS = -Wall -g -O3 -march=native -fopenmp

SOURCE = structured_stack_omp.c
TARGET = $(SOURCE:.c=)

all: $(TARGET)

$(TARGET) : % : %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(TARGET)
