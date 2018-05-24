CC = gcc
CFLAGS = -Wall -std=gnu99
BARBER = barber
CLIENT = client

all: barber.o client.o
	$(CC) $(CFLAGS) $(BARBER).o -o $(BARBER)
	$(CC) $(CFLAGS) $(CLIENT).o -o $(CLIENT)

barber.o: barber.c
	$(CC) $(CFLAGS) -c $(BARBER).c -o $(BARBER).o

client.o: client.c
	$(CC) $(CFLAGS) -c $(CLIENT).c -o $(CLIENT).o

.PHONY: clear

clear:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f $(BARBER)
	rm -f $(CLIENT)