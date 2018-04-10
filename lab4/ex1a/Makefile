CC = gcc
CFLAGS = -Wall -std=gnu99
PROG_NAME = main

.PHONY: clean test

all: main.o
	$(CC) $(CFLAGS) main.o -o main

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

test:
	./main test '<' 2019:01:01 00:00:00



clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main