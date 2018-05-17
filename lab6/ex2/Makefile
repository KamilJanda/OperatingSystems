CC = gcc
CFLAGS = -Wall -std=gnu99
SERVER = server
CLIENT = client

all: server.o client.o
	$(CC) $(CFLAGS) $(SERVER).o -o $(SERVER) -lrt
	$(CC) $(CFLAGS) $(CLIENT).o -o $(CLIENT) -lrt

server.o: server.c
	$(CC) $(CFLAGS) -c $(SERVER).c -o $(SERVER).o

client.o: client.c
	$(CC) $(CFLAGS) -c $(CLIENT).c -o $(CLIENT).o

.PHONY: clear

clear:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f server
	rm -f client