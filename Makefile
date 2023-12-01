.PHONY: all clean

CC = gcc
CFLAGS = -Wall -g -O3 -march=native

SOURCE = naive.c
TARGET = $(SOURCE:.c=)

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(TARGET)
