CC = gcc
CFLAGS=-c -Wall

all: main.o dynamicblocks.o staticcblocks.o
	$(CC) main.o dynamicblocks.o staticcblocks.o -o main

main.o: main.c
	$(CC) $(CFLAGS) main.c

dynamicblocks.o: dynamicblocks.c dynamicblocks.h
	$(CC) $(CFLAGS) dynamicblocks.c

staticcblocks.o: staticcblocks.c staticcblocks.h
	$(CC) $(CFLAGS) staticcblocks.c

clean:
	rm -f *.o main
