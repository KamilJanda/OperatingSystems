CC = gcc
CFLAGS = -Wall -std=gnu99
PROG_NAME = main
LINE = echo "-----------------------------------------------------------------------------" >> wynik.txt
TIME_TEST = time_test

all: main.o time_test
	$(CC) $(CFLAGS) main.o -o main
	

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

time_test: $(TIME_TEST).c
	$(CC) $(CFLAGS) -c $(TIME_TEST).c -o $(TIME_TEST).o
	$(CC) $(CFLAGS) $(TIME_TEST).c -o $(TIME_TEST).o

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
	rm -f time_test
	rm -f memory_test