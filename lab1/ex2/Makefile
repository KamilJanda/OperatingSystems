CC = gcc
CFLAGS = -Wall -std=gnu99
STATIC_BLOCKS = staticcblocks
DYNAMIC_BLOCKS = dynamicblocks

all: static main.o 
	$(CC) $(CFLAGS) main.o lib$(DYNAMIC_BLOCKS).a lib$(STATIC_BLOCKS).a -o main

run: all
	./main b create 1000 1000 find abc addremove 1000000 remove 1000000 add 1000000 | tee raport2.txt

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

static: assembly_static_lib
	ar rcs lib$(DYNAMIC_BLOCKS).a $(DYNAMIC_BLOCKS).o
	ar rcs lib$(STATIC_BLOCKS).a $(STATIC_BLOCKS).o

assembly_static_lib: $(DYNAMIC_BLOCKS).c $(STATIC_BLOCKS).c
	$(CC) $(CFLAGS) -c $(DYNAMIC_BLOCKS).c -o $(DYNAMIC_BLOCKS).o
	$(CC) $(CFLAGS) -c $(STATIC_BLOCKS).c -o $(STATIC_BLOCKS).o

.PHONY: clean

clean:
	rm -f *.o main
	rm -f *.o
	rm -f *.a
	rm -f *.so