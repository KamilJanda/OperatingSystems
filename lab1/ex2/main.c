#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include "dynamicblocks.h"
#include "staticcblocks.h"
#include <string.h> 

#include <unistd.h>

void parse(int argc, char *argv[]);
void process_dynamic(int numberOfOperation, char *operations[numberOfOperation]);
void process_static(int numberOfOperation, char *operations[numberOfOperation]);
void removeAndAddInEveryIterationDynamic(ArrayOfBlocks *dynamicArray, int numberOfAddRemove);
void removeAndAddInEveryIterationStatic(StaticArrayOfBlocks *staticArrayOfBlocks, int numberOfAddRemove,char mainArray[]);
void start_clock(void);
void end_clock(double *results);
void print_results(double *timeCreateFunc, double *timeFindFunc, double *timeAddremoveFunc, double *timeAddFunc, double *timeRemoveFunc, char *type);
int validateCommandArgument(int i, int numberOfOperation, char* operation);

const static long long MAX_SIZE = 1000000;



//variables used to measure duration of operations
static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

int main(int argc, char *argv[])
{
    parse(argc, argv);

    return 0;
}

void parse(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few arguments\n");
        printf("example call: ./main [d/s] [operations]\n");
        exit(0);
    }
    


    char allocationType = (char)argv[1][0];

    int numberOfArgumentsBeforeOperations = 2;

    int numberOfOperation = argc - numberOfArgumentsBeforeOperations;

    char *operations[numberOfOperation];

    for (int i = 0; i < numberOfOperation; i++)
    {
        operations[i] = argv[i + numberOfArgumentsBeforeOperations];
    }

    if (allocationType == 'd')
    {
        process_dynamic(numberOfOperation, operations);
    }
    else if (allocationType == 's')
    {
        process_static(numberOfOperation, operations);
    }
    else
    {
        process_dynamic(numberOfOperation, operations);
        printf("\n\n");
        process_static(numberOfOperation, operations);
    }
}

void process_dynamic(int numberOfOperation, char *operations[numberOfOperation])
{
    ArrayOfBlocks *dynamicArray = NULL;

    int arraySize = 0;
    int blockSize = 0;

    //measure of time results
    double *timeCreateFunc = calloc(3 ,sizeof(double));
    double *timeFindFunc = calloc(3 ,sizeof(double));
    double *timeAddremoveFunc = calloc(3 ,sizeof(double));
    double *timeAddFunc = calloc(3 ,sizeof(double));
    double *timeRemoveFunc = calloc(3 ,sizeof(double));

    for (int i = 0; i < numberOfOperation; i++)
    {
        if (strcmp(operations[i], "create") == 0)
        {
            i++;
            if (i >= numberOfOperation || strcmp(operations[i], "create") == 0 || strcmp(operations[i], "addremove") == 0 || strcmp(operations[i], "find") == 0)
            {
                printf("Invalid argument after find\n");
                return;
            }

            arraySize = atoi(operations[i]);
            i++;
            blockSize = atoi(operations[i]);

            //start time measure
            start_clock();

            dynamicArray = create_array_of_blocks(arraySize, blockSize);

            for (int i = 0; i < dynamicArray->sizeOfArray; i++)
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

            find_block(dynamicArray, sumOfPattern);

            //end time measure
            end_clock(timeFindFunc);

            //printf("pattern: %s found pattern: %s\n", pattern, foundPattern);
            //printf("pattern sum: %d found sum: %d\n\n", sumOfPattern, sum_of_block(foundPattern, dynamicArray->sizeOfBlock));
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
        else if (strcmp(operations[i], "add") == 0)
        {
            i++;
            if (validateCommandArgument(i, numberOfOperation, operations[i]) == 0)
                return;

            int numberOfAdd = atoi(operations[i]);

            //start time measure
            start_clock();

            if (numberOfAdd > dynamicArray->sizeOfArray)
            {
                numberOfAdd = dynamicArray->sizeOfArray;
            }

            for (int i = 0; i < numberOfAdd; i++)
            {
                add_block_with_random_data(dynamicArray, i);
            }

            //end time measure
            end_clock(timeAddFunc);
        }
        else if (strcmp(operations[i], "remove") == 0)
        {
            i++;
            if (validateCommandArgument(i, numberOfOperation, operations[i]) == 0)
                return;

            int numberOfRemove = atoi(operations[i]);

            //start time measure
            start_clock();

            if (numberOfRemove > dynamicArray->sizeOfArray)
            {
                numberOfRemove = dynamicArray->sizeOfArray;
            }

            for (int i = 0; i < numberOfRemove; i++)
            {
                delete_block(dynamicArray, i);
            }

            //end time measure
            end_clock(timeRemoveFunc);
        }
    }

    delete_array_of_blocks(dynamicArray);
    print_results(timeCreateFunc, timeFindFunc, timeAddremoveFunc, timeAddFunc, timeRemoveFunc, "dynamic");
}

void process_static(int numberOfOperation, char *operations[numberOfOperation])
{
    StaticArrayOfBlocks *staticArrayOfBlocks = NULL;
    char mainArray[MAX_SIZE];


    int arraySize = 0;
    int blockSize = 0;

    //measure of time results
    double *timeCreateFunc = calloc(3 ,sizeof(double));
    double *timeFindFunc = calloc(3 ,sizeof(double));
    double *timeAddremoveFunc = calloc(3 ,sizeof(double));
    double *timeAddFunc = calloc(3 ,sizeof(double));
    double *timeRemoveFunc = calloc(3 ,sizeof(double));

    for (int i = 0; i < numberOfOperation; i++)
    {
        if (strcmp(operations[i], "create") == 0)
        {
            i++;
            if (i >= numberOfOperation || strcmp(operations[i], "create") == 0 || strcmp(operations[i], "addremove") == 0 || strcmp(operations[i], "find") == 0)
            {
                printf("Invalid argument after find\n");
                return;
            }

            arraySize = atoi(operations[i]);
            i++;
            blockSize = atoi(operations[i]);

            //start time measure
            start_clock();

            staticArrayOfBlocks = create_array_of_blocks_static(arraySize, blockSize);

            for (int i = 0; i < staticArrayOfBlocks->sizeOfArray; i++)
            {
                addBlockWithRandomDataStatic(staticArrayOfBlocks, i, mainArray);
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

            int sumOfPattern = sum_of_block(pattern, staticArrayOfBlocks->sizeOfBlock);

            //start time measure
            start_clock();

            find_block_static(staticArrayOfBlocks, sumOfPattern, mainArray);

            //end time measure
            end_clock(timeFindFunc);

            //printf("pattern: %s found pattern: %s\n", pattern, foundPattern);
            //printf("pattern sum: %d found sum: %d\n\n", sumOfPattern, sum_of_block(foundPattern, dynamicArray->sizeOfBlock));
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

            removeAndAddInEveryIterationStatic(staticArrayOfBlocks, numberOfAddRemove,mainArray);

            //end time measure
            end_clock(timeAddremoveFunc);
        }
        else if (strcmp(operations[i], "add") == 0)
        {
            i++;
            if (validateCommandArgument(i, numberOfOperation, operations[i]) == 0)
                return;

            int numberOfAdd = atoi(operations[i]);

            //start time measure
            start_clock();

            if (numberOfAdd > staticArrayOfBlocks->sizeOfArray)
            {
                numberOfAdd = staticArrayOfBlocks->sizeOfArray;
            }

            for (int i = 0; i < numberOfAdd; i++)
            {
                addBlockWithRandomDataStatic(staticArrayOfBlocks, i, mainArray);
            }

            //end time measure
            end_clock(timeAddFunc);
        }
        else if (strcmp(operations[i], "remove") == 0)
        {
            i++;
            if (validateCommandArgument(i, numberOfOperation, operations[i]) == 0)
                return;

            int numberOfRemove = atoi(operations[i]);

            //start time measure
            start_clock();

            if (numberOfRemove > staticArrayOfBlocks->sizeOfArray)
            {
                numberOfRemove = staticArrayOfBlocks->sizeOfArray;
            }

            for (int i = 0; i < numberOfRemove; i++)
            {
                delete_block_static(staticArrayOfBlocks, i, mainArray);
            }

            //end time measure
            end_clock(timeRemoveFunc);
        }
    }

    deleteArrayOfBlocksStatic(staticArrayOfBlocks, mainArray);
    print_results(timeCreateFunc, timeFindFunc, timeAddremoveFunc, timeAddFunc, timeRemoveFunc, "static");
}


void removeThenAddNewDynamic(ArrayOfBlocks *dynamicArray, int numberOfAddRemove)
{
    if (numberOfAddRemove > dynamicArray->sizeOfArray)
    {
        numberOfAddRemove = dynamicArray->sizeOfArray;
    }

    for (int i = 0; i < numberOfAddRemove; i++)
    {
        delete_block(dynamicArray, i);
    }

    for (int i = 0; i < numberOfAddRemove; i++)
    {
        add_block_with_random_data(dynamicArray, i);
    }
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
}

void removeAndAddInEveryIterationStatic(StaticArrayOfBlocks *staticArrayOfBlocks, int numberOfAddRemove,char mainArray[])
{

    int *positionsOfAddRemove = malloc(numberOfAddRemove * sizeof(int));

    srand(time(NULL));
    for (int i = 0; i < numberOfAddRemove; i++)
    {
        int r = rand() % (staticArrayOfBlocks->sizeOfArray);
        positionsOfAddRemove[i] = r;
    }

    for (int i = 0; i < numberOfAddRemove; i++)
    {
        delete_block_static(staticArrayOfBlocks, positionsOfAddRemove[i], mainArray);

        addBlockWithRandomDataStatic(staticArrayOfBlocks, positionsOfAddRemove[i], mainArray);
    }
}

void start_clock()
{
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

void print_results(double *timeCreateFunc, double *timeFindFunc, double *timeAddremoveFunc, double *timeAddFunc, double *timeRemoveFunc, char *type)
{
    printf("Create %s table: \n", type);
    printf("Real Time: %f, User Time %f, System Time %f \n\n", timeCreateFunc[0], timeCreateFunc[1], timeCreateFunc[2]);
    printf("Find pattern in %s table: \n", type);
    printf("Real Time: %f, User Time %f, System Time %f \n\n", timeFindFunc[0], timeFindFunc[1], timeFindFunc[2]);
    printf("Add and remove blocks operations in %s table: \n", type);
    printf("Real Time: %f, User Time %f, System Time %f \n\n", timeAddremoveFunc[0], timeAddremoveFunc[1], timeAddremoveFunc[2]);
    printf("Add blocks operations in %s table: \n", type);
    printf("Real Time: %f, User Time %f, System Time %f \n\n", timeAddFunc[0], timeAddFunc[1], timeAddFunc[2]);
    printf("Remove blocks operations in %s table: \n", type);
    printf("Real Time: %f, User Time %f, System Time %f \n\n", timeRemoveFunc[0], timeRemoveFunc[1], timeRemoveFunc[2]);
}

//return 1 if arg is valid, 0 otherwise
int validateCommandArgument(int i, int numberOfOperation, char* operation)
{
    if (i >= numberOfOperation || strcmp(operation, "create") == 0 || strcmp(operation, "addremove") == 0 || strcmp(operation, "find") == 0)
    {
        printf("Invalid argument after %s\n", operation);
        return 0;
    }

    return 1;
}
