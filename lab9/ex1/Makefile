CC = gcc
CFLAGS = -Wall -std=gnu99
PROG_NAME = main
LINE = echo "-----------------------------------------------------------------------------"

all: main.o
	$(CC) $(CFLAGS) main.o -o main -lpthread -lm

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

run:
	./main config.txt
	

.PHONY: clean

clear:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main