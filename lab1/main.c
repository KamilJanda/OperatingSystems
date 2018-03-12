#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include "dynamicblocks.h"

#include <unistd.h>

void create();
void find();
void add_remove_dynamic(ArrayOfBlocks *dynamicArray, int numberOfAddRemove);
void start_clock(void);
void end_clock(double *results);
void print_results(double *timeCreateFunc, double *timeFindFunc, double *timeAddremoveFunc);

//variables used to measure duration of operations
static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

int main(int argc, char *argv[])
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

    char allocationType = argv[3];

    int numberOfOperation = argc - 4;

    char *operations[numberOfOperation];

    for (int i = 0; i < numberOfOperation; i++)
    {
        operations[i] = argv[i + 4];
    }

    //printf("%s \n",operations[0]);

    ArrayOfBlocks *dynamicArray; //= NULL;

    //measure of time results
    double *timeCreateFunc = (double *)malloc(3 * sizeof(double));
    double *timeFindFunc = (double *)malloc(3 * sizeof(double));
    double *timeAddremoveFunc = (double *)malloc(3 * sizeof(double));

    for (int i = 0; i < numberOfOperation; i++)
    {
        if (strcmp(operations[i], "create") == 0)
        {
            //start time measure
            start_clock();

            dynamicArray = create_array_of_blocks(arraySize, blockSize);

            //end time measure
            end_clock(timeCreateFunc);
        }
        else if (strcmp(operations[i], "find") == 0)
        {
            i++;
            if (i >= numberOfOperation || strcmp(operations[i], "create") == 0 || strcmp(operations[i], "addremove") == 0 || strcmp(operations[i], "find") == 0)
            {
                printf("Invalid argument after find\n");
                return 0;
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
        }
        else if (strcmp(operations[i], "addremove") == 0)
        {
            //start time measure
            start_clock();

            i++;
            if (i >= numberOfOperation || strcmp(operations[i], "create") == 0 || strcmp(operations[i], "addremove") == 0 || strcmp(operations[i], "find") == 0)
            {
                printf("Invalid argument after find\n");
                return 0;
            }

            int numberOfAddRemove = atoi(operations[i]);

            //find(dynamicArray, numberOfAddRemove);

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
            }

            for (int i = 0; i < numberOfAddRemove; i++)
            {

                char *block = calloc(dynamicArray->sizeOfBlock, sizeof(char));

                for (int j = 0; j < dynamicArray->sizeOfBlock; j++)
                {
                    int r = rand() % 25 + 97;
                    block[j] = (char)r;
                }

                add_block(dynamicArray, positionsOfAddRemove[i], block);
            }

            //end time measure
            end_clock(timeAddremoveFunc);
        }
    }

    print_results(timeCreateFunc, timeFindFunc, timeAddremoveFunc);

    //ArrayOfBlocks* test = create_array_of_blocks(2,3);

    //fill_in_with_random_data(test);

    //printf("array[1]: %s \n",test -> array[1]);

    return 0;
}

void create()
{
}

void find()
{
}

void add_remove_dynamic(ArrayOfBlocks *dynamicArray, int numberOfAddRemove)
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
    }

    for (int i = 0; i < numberOfAddRemove; i++)
    {

        char *block = calloc(dynamicArray->sizeOfBlock, sizeof(char));

        for (int j = 0; j < dynamicArray->sizeOfBlock; j++)
        {
            int r = rand() % 25 + 97;
            block[j] = (char)r;
        }

        add_block(dynamicArray, positionsOfAddRemove[i], block);
    }
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
