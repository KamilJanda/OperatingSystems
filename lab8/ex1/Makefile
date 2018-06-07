CC = gcc
CFLAGS = -Wall -std=gnu99
PROG_NAME = main
LINE = echo "-----------------------------------------------------------------------------" >> Times.txt

all: main.o
	$(CC) $(CFLAGS) main.o -o main -lpthread -lm

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

run:
	echo > Times.txt
	./main 1 testin.ascii.pgm filter.txt result.pgm | tee -a Times.txt
	./main 2 testin.ascii.pgm filter.txt result.pgm | tee -a Times.txt
	./main 4 testin.ascii.pgm filter.txt result.pgm | tee -a Times.txt
	./main 8 testin.ascii.pgm filter.txt result.pgm | tee -a Times.txt
	$(LINE)
	./main 1 testin.ascii.pgm filter1.txt result1.pgm | tee -a Times.txt
	./main 2 testin.ascii.pgm filter1.txt result1.pgm | tee -a Times.txt
	./main 4 testin.ascii.pgm filter1.txt result1.pgm | tee -a Times.txt
	./main 8 testin.ascii.pgm filter1.txt result1.pgm | tee -a Times.txt


.PHONY: clean

clear:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main