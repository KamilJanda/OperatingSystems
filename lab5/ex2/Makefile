CC = gcc
CFLAGS = -Wall -std=gnu99
MASTER = master
SLAVE = slave

all: master.o slave.o
	$(CC) $(CFLAGS) master.o -o master
	$(CC) $(CFLAGS) slave.o -o slave

master.o: master.c
	$(CC) $(CFLAGS) -c master.c -o master.o

slave.o: slave.c
	$(CC) $(CFLAGS) -c slave.c -o slave.o

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f master
	rm -f slave