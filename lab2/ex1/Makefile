CC = gcc
CFLAGS = -Wall -std=gnu99
FILE_OPERATIONS = fileoperations


all: main.o static_lib
	$(CC) $(CFLAGS) main.o lib$(FILE_OPERATIONS).a -o main

run: main.o clean

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

assembly_static_lib: $(FILE_OPERATIONS).c
	$(CC) $(CFLAGS) -c $(FILE_OPERATIONS).c -o $(FILE_OPERATIONS).o

static_lib: assembly_static_lib
	ar rcs lib$(FILE_OPERATIONS).a $(FILE_OPERATIONS).o	


.PHONY: clean

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so