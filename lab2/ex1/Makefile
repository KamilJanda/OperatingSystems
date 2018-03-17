CC = gcc
CFLAGS = -Wall -std=gnu99


all: main.o
	$(CC) $(CFLAGS) main.o -o main

run: main.o clean

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so