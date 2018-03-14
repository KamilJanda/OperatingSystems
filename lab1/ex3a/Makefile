CC = gcc
CFLAGS = -Wall
STATIC_BLOCKS = staticcblocks
DYNAMIC_BLOCKS = dynamicblocks
SHARED_LIB_ARG =
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	SHARED_LIB_ARG += -L. -Wl,-rpath=.
endif

all: dynamic

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

static: assembly_static_lib static_archive main.o
	$(CC) $(CFLAGS) main.o lib$(DYNAMIC_BLOCKS).a lib$(STATIC_BLOCKS).a -o main_static	

assembly_static_lib: $(DYNAMIC_BLOCKS).c $(STATIC_BLOCKS).c
	$(CC) $(CFLAGS) -c $(DYNAMIC_BLOCKS).c -o $(DYNAMIC_BLOCKS).o
	$(CC) $(CFLAGS) -c $(STATIC_BLOCKS).c -o $(STATIC_BLOCKS).o

static_archive: assembly_static_lib
	ar rcs lib$(DYNAMIC_BLOCKS).a $(DYNAMIC_BLOCKS).o
	ar rcs lib$(STATIC_BLOCKS).a $(STATIC_BLOCKS).o

shared: $(DYNAMIC_BLOCKS).c $(STATIC_BLOCKS).c main.c
	$(CC) $(CFLAGS) -fPIC -shared $(DYNAMIC_BLOCKS).c -o lib$(DYNAMIC_BLOCKS).so
	$(CC) $(CFLAGS) -fPIC -shared $(STATIC_BLOCKS).c -o lib$(STATIC_BLOCKS).so
	$(CC) $(CFLAGS) main.c $(SHARED_LIB_ARG) -l$(STATIC_BLOCKS) -l$(DYNAMIC_BLOCKS) -L ./ -o main_shared

dynamic: $(DYNAMIC_BLOCKS).c $(STATIC_BLOCKS).c main.c
	$(CC) $(CFLAGS) -fPIC -shared $(DYNAMIC_BLOCKS).c -o lib$(DYNAMIC_BLOCKS).so
	$(CC) $(CFLAGS) -fPIC -shared $(STATIC_BLOCKS).c -o lib$(STATIC_BLOCKS).so
	$(CC) $(CFLAGS) main.c -o main_dynamic -ldl -D DYNAMIC 


.PHONY: clean

clean:
	rm -f *.o main_*
	rm -f *.o
	rm -f *.a
	rm -f *.so