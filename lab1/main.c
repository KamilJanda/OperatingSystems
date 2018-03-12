#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include "dynamicblocks.h"

#include <unistd.h> 

void createArray();
void start_clock(void);
void end_clock(double *results);
void print_results(double* timeCreateFunc);


//variables used to measure duration of operations 
static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

int main( int argc, char *argv[])  {

    if(argc<5)
    {
        printf("Too few arguments\n");
        printf("example call: ./main [size of array] [size of blocks] [d/s] [operations]\n");
        exit(0);
    }

    int arraySize = atoi(argv[1]);
    int blockSize = atoi(argv[2]);

    //printf("array: %d, block: %d \n",arraySize,blockSize);

    char allocationType = argv[3];

    int numberOfOperation = argc - 4;

    char* operations[numberOfOperation];

    for(int i=0;i<numberOfOperation;i++)
    {
        operations[i] = argv[i+4];
    }


    printf("%s \n",operations[0]);

    ArrayOfBlocks* dynamicArray; //= NULL;    

    //measure of time results
    double* timeCreateFunc =(double*) malloc(3*sizeof(double));


    for(int i=0;i<numberOfOperation;i++)
    {
        if(strcmp(operations[i],"create") == 0)
        {
            //start time measure
            start_clock();

            dynamicArray = create_array_of_blocks(arraySize,blockSize);

            //end time measure
            end_clock(timeCreateFunc);
        }
        else if(strcmp(operations[i],"find") == 0)
        {

        }
        else if(strcmp(operations[i],"addremove") == 0)
        {
            
        }


    }

    
    print_results(timeCreateFunc);

    //ArrayOfBlocks* test = create_array_of_blocks(2,3);

    //fill_in_with_random_data(test);

    //printf("array[1]: %s \n",test -> array[1]);

    return 0;
}

void createArray()
{

}

void start_clock()
{
    printf("st_time: %d \n",st_time);
    st_time = times(&st_cpu);
}



void end_clock(double *results)
{
    en_time = times(&en_cpu);

    double clk = sysconf(_SC_CLK_TCK);

    results[0] = (en_time - st_time) / clk;
    results[1] = (en_cpu.tms_utime - st_cpu.tms_utime) / clk;
    results[2] = (en_cpu.tms_stime - st_cpu.tms_stime) / clk;
}

void print_results(double* timeCreateFunc)
{
    printf("Create dynamic table: \n");
    printf("Real Time: %f, User Time %f, System Time %f \n",timeCreateFunc[0],timeCreateFunc[1],timeCreateFunc[2]);

}

