CC = gcc

all: main.o dynamicblocks.o
    $(CC) main.o dynamicblocks.o -o main

main.o: main.c
    $(CC) main.c -c

dynamicblocks.o: dynamicblocks.c dynamicblocks.h
    $(CC) dynamicblocks.c -c -o dynamicblocks.h

clean:
  rm -f *.o main