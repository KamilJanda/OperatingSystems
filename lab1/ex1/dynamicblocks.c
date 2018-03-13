//
// Created by Kamil on 11.03.2018.
//

#include "dynamicblocks.h"
#include <stdlib.h>
#include <math.h>
#include <limits.h>

ArrayOfBlocks *create_array_of_blocks(int sizeOfArray, int sizeOfBlock)
{
    ArrayOfBlocks *newArrayOfBlocks = malloc(sizeof(ArrayOfBlocks));

    newArrayOfBlocks->sizeOfArray = sizeOfArray;
    newArrayOfBlocks->sizeOfBlock = sizeOfBlock;
    newArrayOfBlocks->array = calloc(sizeOfArray, sizeof(char *));

    //srand(time(NULL));

    for (int i = 0; i < sizeOfArray; i++)
    {
        newArrayOfBlocks->array[i] = NULL; //calloc(sizeOfBlock, sizeof(char));

        /*
        for (int j = 0; j < newArrayOfBlocks->sizeOfBlock; j++)
        {
            int r = rand() % 25 + 97;
            newArrayOfBlocks->array[i][j] = (char)r;
        }
        */
    }

    return newArrayOfBlocks;
}

void delete_array_of_blocks(ArrayOfBlocks *arrayOfBlocks)
{
    
    for (int i = 0; i < arrayOfBlocks->sizeOfArray; i++)
    {
        free(arrayOfBlocks->array[i]);
    }

    free(arrayOfBlocks->array);

    free(arrayOfBlocks);
}

void add_block(ArrayOfBlocks *arrayOfBlocks, int index, char *block,int blockSize)
{
    if (index >= arrayOfBlocks->sizeOfArray)
    {
        printf("Error: index is out of range");
        return;
    }
    if (index < 0)
    {
        printf("Error: index cannot be negative number");
        return;
    }
    if (arrayOfBlocks == NULL)
    {
        printf("Error: array of blocks is NULL");
        return;
    }
    if (arrayOfBlocks->array[index] != NULL)
    {
        printf("Error: selected index is not empty");
        return;
    }
    if(blockSize != arrayOfBlocks->sizeOfBlock)
    {
        printf("Error: block size is incorrect");
        return;
    }

    arrayOfBlocks->array[index] = block;
}

void add_block_with_random_data(ArrayOfBlocks *arrayOfBlocks, int index)
{
    if (index >= arrayOfBlocks->sizeOfArray)
    {
        printf("Error: index is out of range");
        return;
    }
    if (index < 0)
    {
        printf("Error: index cannot be negative number");
        return;
    }
    if (arrayOfBlocks == NULL)
    {
        printf("Error: array of blocks is NULL");
        return;
    }
    if (arrayOfBlocks->array[index] != NULL)
    {
        printf("Error: selected index is not empty");
        return;
    }

    arrayOfBlocks->array[index] = calloc(arrayOfBlocks->sizeOfBlock, sizeof(char));

    for (int j = 0; j < arrayOfBlocks->sizeOfBlock; j++)
    {
        int r = rand() % 25 + 97;
        arrayOfBlocks->array[index][j] = (char)r;
       
    }
    
}

void delete_block(ArrayOfBlocks *arrayOfBlocks, int index)
{
    if (index >= arrayOfBlocks->sizeOfArray)
    {
        printf("Error: index is out of range");
        return;
    }
    if (index < 0)
    {
        printf("Error: index cannot be negative number");
        return;
    }
    if (arrayOfBlocks == NULL)
    {
        printf("Error: array of blocks is NULL");
        return;
    }
    
    free(arrayOfBlocks->array[index]);
    arrayOfBlocks->array[index] = NULL;

}

char *find_block(ArrayOfBlocks *arrayOfBlocks, int sum)
{
    int tmpSum = 0;
    int indexOfMinDiff = -1;
    int minDifference = INT_MAX;
    for (int i = 0; i < arrayOfBlocks->sizeOfArray; i++)
    {
        tmpSum = sum_of_block(arrayOfBlocks->array[i], arrayOfBlocks->sizeOfBlock);

        int diff = abs(sum - tmpSum);

        if (diff < minDifference)
        {
            minDifference = diff;
            indexOfMinDiff = i;
        }
    }

    return arrayOfBlocks->array[indexOfMinDiff];
}

int sum_of_block(char* block, int size)
{
    if(block == NULL) return 0;

    int sum = 0;
    for (int i = 0; i < size; i++)
    {
        if (block[i] == '\0')
            return sum;

        sum += (int)block[i];
    }
    return sum;
}

