#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <dlfcn.h>

#ifndef DYNAMIC
#include "dynamicblocks.h"
#include "staticcblocks.h"
#endif

//function from dynamicblocks library
#ifdef DYNAMIC
typedef struct
{
    char **array;
    int sizeOfArray;
    int sizeOfBlock;
} ArrayOfBlocks;

ArrayOfBlocks *(*create_array_of_blocks)(int sizeOfArray, int sizeOfBlock);

void (*delete_array_of_blocks)(ArrayOfBlocks *arrayOfBlocks);

void (*add_block)(ArrayOfBlocks *arrayOfBlocks, int index, char *block, int blockSize);

void (*add_block_with_random_data)(ArrayOfBlocks *arrayOfBlocks, int index);

void (*delete_block)(ArrayOfBlocks *arrayOfBlocks, int index);

char *(*find_block)(ArrayOfBlocks *arrayOfBlocks, int sum);

int (*sum_of_block)(char *block, int size);
#endif

//function from staticblocks library
#ifdef DYNAMIC
typedef struct
{
    int sizeOfArray;
    int sizeOfBlock;
    int *blockAvailabilityMap;
} StaticArrayOfBlocks;

StaticArrayOfBlocks *(*create_array_of_blocks_static)(int sizeOfArray, int sizeOfBlock);

void (*deleteArrayOfBlocksStatic)(StaticArrayOfBlocks *staticArrayOfBlocks, char array[]);

void (*add_block_static)(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char block[], int blockSize, char array[]);

void (*addBlockWithRandomDataStatic)(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char array[]);

void (*delete_block_static)(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char array[]);

int (*find_block_static)(StaticArrayOfBlocks *staticArrayOfBlocks, int sum, char array[]);

#endif


void parse(int argc, char *argv[]);
void process_dynamic(int numberOfOperation, char *operations[numberOfOperation]);
void process_static(int numberOfOperation, char *operations[numberOfOperation]);
void removeAndAddInEveryIterationDynamic(ArrayOfBlocks *dynamicArray, int numberOfAddRemove);
void removeAndAddInEveryIterationStatic(StaticArrayOfBlocks *staticArrayOfBlocks, int numberOfAddRemove);
void start_clock(void);
void end_clock(double *results);
void print_results(double *timeCreateFunc, double *timeFindFunc, double *timeAddremoveFunc, double *timeAddFunc, double *timeRemoveFunc, char *type);
int validateCommandArgument(int i, int numberOfOperation, char *operation);



const static long long maxSize = 1000000;

char GLOBAL_ARRAY[maxSize];

//variables used to measure duration of operations
static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

int main(int argc, char *argv[])
{
#ifdef DYNAMIC
    void *handleDynamic = dlopen("./libdynamicblocks.so", RTLD_LAZY);
    void *handleStatic = dlopen("./libstaticcblocks.so", RTLD_LAZY);

    create_array_of_blocks = dlsym(handleDynamic, "create_array_of_blocks");
    delete_array_of_blocks = dlsym(handleDynamic, "delete_array_of_blocks");
    add_block = dlsym(handleDynamic, "add_block");
    add_block_with_random_data = dlsym(handleDynamic, "add_block_with_random_data");
    delete_block = dlsym(handleDynamic, "delete_block");
    find_block = dlsym(handleDynamic, "find_block");
    sum_of_block = dlsym(handleDynamic, "sum_of_block");

    create_array_of_blocks_static = dlsym(handleStatic, "create_array_of_blocks_static");
    deleteArrayOfBlocksStatic = dlsym(handleStatic, "deleteArrayOfBlocksStatic");
    add_block_static = dlsym(handleStatic, "add_block_static");
    addBlockWithRandomDataStatic = dlsym(handleStatic, "addBlockWithRandomDataStatic");
    delete_block_static = dlsym(handleStatic, "delete_block_static");
    find_block_static = dlsym(handleStatic, "find_block_static");

#endif

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
    double *timeCreateFunc = calloc(3, sizeof(double));
    double *timeFindFunc = calloc(3, sizeof(double));
    double *timeAddremoveFunc = calloc(3, sizeof(double));
    double *timeAddFunc = calloc(3, sizeof(double));
    double *timeRemoveFunc = calloc(3, sizeof(double));

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

            char *foundPattern = find_block(dynamicArray, sumOfPattern);

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

    int arraySize = 0;
    int blockSize = 0;

    //measure of time results
    double *timeCreateFunc = calloc(3, sizeof(double));
    double *timeFindFunc = calloc(3, sizeof(double));
    double *timeAddremoveFunc = calloc(3, sizeof(double));
    double *timeAddFunc = calloc(3, sizeof(double));
    double *timeRemoveFunc = calloc(3, sizeof(double));

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
                addBlockWithRandomDataStatic(staticArrayOfBlocks, i, GLOBAL_ARRAY);
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

            char *foundPattern = find_block_static(staticArrayOfBlocks, sumOfPattern, GLOBAL_ARRAY);

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

            removeAndAddInEveryIterationStatic(staticArrayOfBlocks, numberOfAddRemove);

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
                addBlockWithRandomDataStatic(staticArrayOfBlocks, i, GLOBAL_ARRAY);
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
                delete_block_static(staticArrayOfBlocks, i, GLOBAL_ARRAY);
            }

            //end time measure
            end_clock(timeRemoveFunc);
        }
    }

    deleteArrayOfBlocksStatic(staticArrayOfBlocks, GLOBAL_ARRAY);
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

void removeAndAddInEveryIterationStatic(StaticArrayOfBlocks *staticArrayOfBlocks, int numberOfAddRemove)
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
        delete_block_static(staticArrayOfBlocks, positionsOfAddRemove[i], GLOBAL_ARRAY);

        addBlockWithRandomDataStatic(staticArrayOfBlocks, positionsOfAddRemove[i], GLOBAL_ARRAY);
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
int validateCommandArgument(int i, int numberOfOperation, char *operation)
{
    if (i >= numberOfOperation || strcmp(operation, "create") == 0 || strcmp(operation, "addremove") == 0 || strcmp(operation, "find") == 0)
    {
        printf("Invalid argument after %s\n", operation);
        return 0;
    }

    return 1;
}
