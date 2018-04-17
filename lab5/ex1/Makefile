CC = gcc
CFLAGS = -Wall -std=gnu99
PROG_NAME = main
LINE = echo "-----------------------------------------------------------------------------" >> wynik.txt

all: main.o
	$(CC) $(CFLAGS) main.o -o main

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main