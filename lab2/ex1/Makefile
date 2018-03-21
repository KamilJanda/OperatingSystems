CC = gcc
CFLAGS = -Wall -std=gnu99
FILE_OPERATIONS = fileoperations
TIME_UTIL = timeutil
PROG_NAME = main
LINE = echo "-----------------------------------------------------------------------------" >> wynik.txt
SIZE1 = 1000
SIZE2 = 2000


all: main.o static_lib
	$(CC) $(CFLAGS) main.o lib$(FILE_OPERATIONS).a lib$(TIME_UTIL).a -o main

run: all clean_data clean
	echo "record: $(SIZE1) size: 4" > wynik.txt
	./$(PROG_NAME) generate data $(SIZE1) 4 | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE1) 4 sys | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE1) 4 lib | tee -a wynik.txt
	./$(PROG_NAME) copy data data_copy $(SIZE1) 4 sys | tee -a wynik.txt
	rm data_copy
	./$(PROG_NAME) copy data data_copy $(SIZE1) 4 lib | tee -a wynik.txt
	make clean_data

	$(LINE)
	echo "record: $(SIZE2) size: 4" >> wynik.txt
	./$(PROG_NAME) generate data $(SIZE2) 4 | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE2) 4 sys | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE2) 4 lib | tee -a wynik.txt
	./$(PROG_NAME) copy data data_copy $(SIZE2) 4 sys | tee -a wynik.txt
	rm data_copy
	./$(PROG_NAME) copy data data_copy $(SIZE2) 4 lib | tee -a wynik.txt
	make clean_data

	$(LINE)
	echo "record: $(SIZE1) size: 512" >> wynik.txt
	./$(PROG_NAME) generate data $(SIZE1) 512 | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE1) 512 sys | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE1) 512 lib | tee -a wynik.txt
	./$(PROG_NAME) copy data data_copy $(SIZE1) 512 sys | tee -a wynik.txt
	rm data_copy
	./$(PROG_NAME) copy data data_copy $(SIZE1) 512 lib | tee -a wynik.txt
	make clean_data

	$(LINE)
	echo "record: $(SIZE2) size: 512" >> wynik.txt
	./$(PROG_NAME) generate data $(SIZE2) 512 | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE2) 512 sys | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE2) 512 lib | tee -a wynik.txt
	./$(PROG_NAME) copy data data_copy $(SIZE2) 512 sys | tee -a wynik.txt
	rm data_copy
	./$(PROG_NAME) copy data data_copy $(SIZE2) 512 lib | tee -a wynik.txt
	make clean_data

	$(LINE)
	echo "record: $(SIZE1) size: 4096" >> wynik.txt
	./$(PROG_NAME) generate data $(SIZE1) 4096 | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE1) 4096 sys | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE1) 4096 lib | tee -a wynik.txt
	./$(PROG_NAME) copy data data_copy $(SIZE1) 4096 sys | tee -a wynik.txt
	rm data_copy
	./$(PROG_NAME) copy data data_copy $(SIZE1) 4096 lib | tee -a wynik.txt
	make clean_data

	$(LINE)
	echo "record: $(SIZE2) size: 4096" >> wynik.txt
	./$(PROG_NAME) generate data $(SIZE2) 4096 | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE2) 4096 sys | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE2) 4096 lib | tee -a wynik.txt
	./$(PROG_NAME) copy data data_copy $(SIZE2) 4096 sys | tee -a wynik.txt
	rm data_copy
	./$(PROG_NAME) copy data data_copy $(SIZE2) 4096 lib | tee -a wynik.txt
	make clean_data

	$(LINE)
	echo "record: $(SIZE1) size: 8192" >> wynik.txt
	./$(PROG_NAME) generate data $(SIZE1) 8192 | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE1) 8192 sys | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE1) 8192 lib | tee -a wynik.txt
	./$(PROG_NAME) copy data data_copy $(SIZE1) 8192 sys | tee -a wynik.txt
	rm data_copy
	./$(PROG_NAME) copy data data_copy $(SIZE1) 8192 lib | tee -a wynik.txt
	make clean_data

	$(LINE)
	echo "record: $(SIZE2) size: 8192" >> wynik.txt
	./$(PROG_NAME) generate data $(SIZE2) 8192 | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE2) 8192 sys | tee -a wynik.txt
	cp data data_sort
	./$(PROG_NAME) sort data_sort $(SIZE2) 8192 lib | tee -a wynik.txt
	./$(PROG_NAME) copy data data_copy $(SIZE2) 8192 sys | tee -a wynik.txt
	rm data_copy
	./$(PROG_NAME) copy data data_copy $(SIZE2) 8192 lib | tee -a wynik.txt
	make clean_data



	make clean

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

assembly_static_lib: $(FILE_OPERATIONS).c
	$(CC) $(CFLAGS) -c $(FILE_OPERATIONS).c -o $(FILE_OPERATIONS).o
	$(CC) $(CFLAGS) -c $(TIME_UTIL).c -o $(TIME_UTIL).o

static_lib: assembly_static_lib
	ar rcs lib$(FILE_OPERATIONS).a $(FILE_OPERATIONS).o	
	ar rcs lib$(TIME_UTIL).a $(TIME_UTIL).o	

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so

clean_data:
	rm -f data*