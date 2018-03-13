#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include "dynamicblocks.h"

#include <unistd.h>


void parse(int argc, char *argv[]);
void process_dynamic(int numberOfOperation,char* operations[numberOfOperation],int arraySize,int blockSize);
void create();
void find();
void removeAndAddInEveryIterationDynamic(ArrayOfBlocks *dynamicArray, int numberOfAddRemove);
void start_clock(void);
void end_clock(double *results);
void print_results(double *timeCreateFunc, double *timeFindFunc, double *timeAddremoveFunc);
int tmp(char* operation);

const static int maxSize = 100000000;

char arr[maxSize];

//variables used to measure duration of operations
static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

int main(int argc, char *argv[])
{
    //parse(argc, argv);

    

    return 0;
}

void parse(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("Too few arguments\n");
        printf("example call: ./main [size of array] [size of blocks] [d/s] [operations]\n");
        exit(0);
    }

    int arraySize = atoi(argv[1]);
    int blockSize = atoi(argv[2]);

    //printf("array: %d, block: %d \n",arraySize,blockSize);

    char allocationType = (char)argv[3][0];

    int numberOfOperation = argc - 4;

    char *operations[numberOfOperation];

    for (int i = 0; i < numberOfOperation; i++)
    {
        operations[i] = argv[i + 4];
    }

    printf("%s \n",argv[3]);
    printf("%c \n",allocationType);

    if(allocationType == 'd')
    {
        process_dynamic(numberOfOperation,operations,arraySize,blockSize);
    }
    else if(allocationType == 's')
    {

    }
    else
    {

    }

    

}

void process_dynamic(int numberOfOperation,char* operations[numberOfOperation],int arraySize,int blockSize)
{
    ArrayOfBlocks *dynamicArray; //= NULL;

    //measure of time results
    double *timeCreateFunc = (double *)malloc(3 * sizeof(double));
    double *timeFindFunc = (double *)malloc(3 * sizeof(double));
    double *timeAddremoveFunc = (double *)malloc(3 * sizeof(double));

    for (int i = 0; i < numberOfOperation; i++)
    {
        if (strcmp(operations[i], "create") == 0)
        {
            /*
            i++;
            if (i >= numberOfOperation || tmp(operations[i]) == 1)
            {
                printf("Invalid argument after create\n");
                return;
            }
            */


            //start time measure
            start_clock();

            dynamicArray = create_array_of_blocks(arraySize, blockSize);

            for(int i=0;i<dynamicArray->sizeOfArray;i++)
            {
                add_block_with_random_data(dynamicArray, i);
            }

            //end time measure
            end_clock(timeCreateFunc);
        }
        else if (strcmp(operations[i], "find") == 0)
        {
            i++;
            if (i >= numberOfOperation || strcmp(operations[i], "create") == 0 || strcmp(operations[i], "addremove") == 0 || strcmp(operations[i], "find") == 0)
            {
                printf("Invalid argument after find\n");
                return;
            }

            char *pattern = operations[i];

            int sumOfPattern = sum_of_block(pattern, dynamicArray->sizeOfBlock);



            //start time measure
            start_clock();


            char *foundPattern = find_block(dynamicArray, sumOfPattern);

            //end time measure
            end_clock(timeFindFunc);

            printf("pattern: %s found pattern: %s\n", pattern, foundPattern);
            printf("pattern sum: %d found sum: %d\n\n", sumOfPattern, sum_of_block(foundPattern, dynamicArray->sizeOfBlock));

            free(foundPattern);
        }
        else if (strcmp(operations[i], "addremove") == 0)
        {
            i++;
            if (i >= numberOfOperation || strcmp(operations[i], "create") == 0 || strcmp(operations[i], "addremove") == 0 || strcmp(operations[i], "find") == 0)
            {
                printf("Invalid argument after addremove\n");
                return;
            }

            int numberOfAddRemove = atoi(operations[i]);

            //start time measure
            start_clock();

            removeAndAddInEveryIterationDynamic(dynamicArray, numberOfAddRemove);

            //end time measure
            end_clock(timeAddremoveFunc);
        }
    }

    print_results(timeCreateFunc, timeFindFunc, timeAddremoveFunc);
}

void process_static()
{

}

void create()
{
}

void find()
{
}

void removeThenAddNewDynamic(ArrayOfBlocks *dynamicArray, int numberOfAddRemove)
{
    if(numberOfAddRemove > dynamicArray->sizeOfArray)
    {
        numberOfAddRemove = dynamicArray->sizeOfArray;
    }

    for (int i = 0; i < numberOfAddRemove; i++)
    {
        delete_block(dynamicArray, i);
    }

    printf("TUTAJ2\n");

    for (int i = 0; i < numberOfAddRemove; i++)
    {
        add_block_with_random_data(dynamicArray, i);
    }

     printf("TUTAJ3\n");
}


void removeAndAddInEveryIterationDynamic(ArrayOfBlocks *dynamicArray, int numberOfAddRemove)
{
    int *positionsOfAddRemove = malloc(numberOfAddRemove * sizeof(int));
    

    srand(time(NULL));
    for (int i = 0; i < numberOfAddRemove; i++)
    {
        int r = rand() % dynamicArray->sizeOfArray;
        positionsOfAddRemove[i] = r;
    }

    for (int i = 0; i < numberOfAddRemove; i++)
    {
        delete_block(dynamicArray, positionsOfAddRemove[i]);
        add_block_with_random_data(dynamicArray, positionsOfAddRemove[i]);
    }
    /*
    for (int i = 0; i < numberOfAddRemove; i++)
    {

        char *block = calloc(dynamicArray->sizeOfBlock, sizeof(char));

        for (int j = 0; j < dynamicArray->sizeOfBlock; j++)
        {
            int r = rand() % 25 + 97;
            block[j] = (char)r;
        }

        add_block(dynamicArray, positionsOfAddRemove[i], block,dynamicArray->sizeOfBlock);
    }

    */
}

void start_clock()
{
    //printf("st_time: %d \n",st_time);
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

void print_results(double *timeCreateFunc, double *timeFindFunc, double *timeAddremoveFunc)
{
    printf("Create dynamic table: \n");
    printf("Real Time: %f, User Time %f, System Time %f \n", timeCreateFunc[0], timeCreateFunc[1], timeCreateFunc[2]);
    printf("Find pattern in dynamic table: \n");
    printf("Real Time: %f, User Time %f, System Time %f \n", timeFindFunc[0], timeFindFunc[1], timeFindFunc[2]);
    printf("Add and remove blocks operations in dynamic table: \n");
    printf("Real Time: %f, User Time %f, System Time %f \n", timeAddremoveFunc[0], timeAddremoveFunc[1], timeAddremoveFunc[2]);
}

// return 1 if operation arg is next operation 0 otherwise
int tmp(char* operation)
{
    if ((strcmp(operation, "create") == 0) || (strcmp(operation, "addremove") == 0) || (strcmp(operation, "find") == 0))
    {
        return 1;
    }
    return 0;
}
