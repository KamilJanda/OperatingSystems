CC = gcc
CFLAGS = -Wall
STATIC_BLOCKS = staticcblocks
DYNAMIC_BLOCKS = dynamicblocks

all: static shared clean

static: assembly_static_lib
	ar rcs lib$(DYNAMIC_BLOCKS).a $(DYNAMIC_BLOCKS).o
	ar rcs lib$(STATIC_BLOCKS).a $(STATIC_BLOCKS).o

shared: assembly_shared_lib
	$(CC) $(CFLAGS) -shared -o lib$(DYNAMIC_BLOCKS).so $(DYNAMIC_BLOCKS).o
	$(CC) $(CFLAGS) -shared -o lib$(STATIC_BLOCKS).so $(STATIC_BLOCKS).o

assembly_static_lib: $(DYNAMIC_BLOCKS).c $(STATIC_BLOCKS).c
	$(CC) $(CFLAGS) -c $(DYNAMIC_BLOCKS).c -o $(DYNAMIC_BLOCKS).o
	$(CC) $(CFLAGS) -c $(STATIC_BLOCKS).c -o $(STATIC_BLOCKS).o

assembly_shared_lib: $(DYNAMIC_BLOCKS).c $(STATIC_BLOCKS).c
	$(CC) $(CFLAGS) -fPIC -c $(DYNAMIC_BLOCKS).c -o $(DYNAMIC_BLOCKS).o
	$(CC) $(CFLAGS) -fPIC -c $(STATIC_BLOCKS).c -o $(STATIC_BLOCKS).o

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so