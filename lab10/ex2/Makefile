CC = gcc
CFLAGS = -Wall -std=gnu99
SERVER = server
CLIENT = client

all: $(SERVER).o client.o
	$(CC) $(CFLAGS) $(SERVER).o -o $(SERVER) -lpthread -lm
	$(CC) $(CFLAGS) $(CLIENT).o -o $(CLIENT)

$(SERVER).o: $(SERVER).c
	$(CC) $(CFLAGS) -c $(SERVER).c -o $(SERVER).o

client.o: client.c
	$(CC) $(CFLAGS) -c $(CLIENT).c -o $(CLIENT).o

.PHONY: clear

s:
	./server 8083 ala

c:
	./client ala local ala

clear:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f $(SERVER)
	rm -f $(CLIENT)