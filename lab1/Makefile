CC = gcc
CFLAGS=-c -Wall

all: main.o dynamicblocks.o
	$(CC) main.o dynamicblocks.o -o main

main.o: main.c
	$(CC) $(CFLAGS) main.c

dynamicblocks.o: dynamicblocks.c dynamicblocks.h
	$(CC) $(CFLAGS) dynamicblocks.c

clean:
	rm -f *.o main
