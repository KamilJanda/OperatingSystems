CC = gcc
CFLAGS = -Wall -std=gnu99
PROG_NAME = main
LINE = echo "-----------------------------------------------------------------------------" >> wynik.txt
TIME_TEST = time_test
MEMORY_TEST = memory_test

TIME_LIMIT = 2
MEM_LIMIT =  10

.PHONY: clean test

all: clean main.o time_test memory_test
	$(CC) $(CFLAGS) main.o -o main
	

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

time_test: $(TIME_TEST).c
	$(CC) $(CFLAGS) -c $(TIME_TEST).c -o $(TIME_TEST).o
	$(CC) $(CFLAGS) $(TIME_TEST).c -o $(TIME_TEST)

memory_test: $(MEMORY_TEST).c
	$(CC) $(CFLAGS) -c $(MEMORY_TEST).c -o $(MEMORY_TEST).o
	$(CC) $(CFLAGS) $(MEMORY_TEST).c -o $(MEMORY_TEST)

test: 
	./main test $(TIME_LIMIT) $(MEM_LIMIT)


clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
	rm -f time_test
	rm -f memory_test