//
// Created by Kamil on 11.03.2018.
//

#include "staticcblocks.h"
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdio.h> 

StaticArrayOfBlocks *create_array_of_blocks_static(int sizeOfArray, int sizeOfBlock)
{
    StaticArrayOfBlocks *staticArrayOfBlocks = malloc(sizeof(StaticArrayOfBlocks));

    staticArrayOfBlocks->sizeOfArray = sizeOfArray;
    staticArrayOfBlocks->sizeOfBlock = sizeOfBlock;
    staticArrayOfBlocks->blockAvailabilityMap = calloc(sizeOfArray, sizeof(int));

    return staticArrayOfBlocks;
}

void deleteArrayOfBlocksStatic(StaticArrayOfBlocks *staticArrayOfBlocks, char array[])
{
    int size = (staticArrayOfBlocks->sizeOfArray) * (staticArrayOfBlocks->sizeOfBlock);

    for (int i = 0; i < size; i++)
    {
        array[i] = ' ';
    }

    free(staticArrayOfBlocks);
}

void add_block_static(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char block[], int blockSize, char array[])
{

    if (index >= staticArrayOfBlocks->sizeOfArray)
    {
        printf("Error: index is out of range");
        return;
    }
    if (index < 0)
    {
        printf("Error: index cannot be negative number");
        return;
    }

    if (staticArrayOfBlocks->blockAvailabilityMap == NULL)
        
        if (staticArrayOfBlocks->blockAvailabilityMap[index] == 1)
        {
            printf("Error: Block with given index is not empty");
            return;
        }

    if (staticArrayOfBlocks == NULL)
    {
        printf("Error:  staticArrayOfBlocks is NULL");
        return;
    }
    if (blockSize != staticArrayOfBlocks->sizeOfBlock)
    {
        printf("Error: block size is incorrect");
        return;
    }

    staticArrayOfBlocks->blockAvailabilityMap[index] = 1;

    int position;
    for (int i = 0; i < staticArrayOfBlocks->sizeOfBlock; i++)
    {
        position = (index * (staticArrayOfBlocks->sizeOfBlock)) + i;

        array[position] = block[i];
    }
}

void addBlockWithRandomDataStatic(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char array[])
{
    char *block = (char *)malloc((staticArrayOfBlocks->sizeOfBlock) * sizeof(char));

    for (int i = 0; i < staticArrayOfBlocks->sizeOfBlock; i++)
    {
        int r = rand() % 25 + 97;
        block[i] = (char)r;
    }

    add_block_static(staticArrayOfBlocks, index, block, staticArrayOfBlocks->sizeOfBlock, array);
}

void delete_block_static(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char array[])
{
    if (index >= staticArrayOfBlocks->sizeOfArray)
    {
        printf("Error from delete_block_static: index is out of range \n");
        return;
    }
    if (index < 0)
    {
        printf("Error from delete_block_static: index cannot be negative number \n");
        return;
    }
    if (staticArrayOfBlocks == NULL)
    {
        printf("Error from delete_block_static: array of blocks is NULL\n");
        return;
    }

    staticArrayOfBlocks->blockAvailabilityMap[index] = 0;

    int position;
    for (int i = 0; i < staticArrayOfBlocks->sizeOfBlock; i++)
    {
        position = (index * staticArrayOfBlocks->sizeOfBlock) + i;
        array[position] = ' ';
    }
}

int find_block_static(StaticArrayOfBlocks *staticArrayOfBlocks, int sum, char array[])
{
    int tmpSum = 0;
    int indexOfMinDiff = -1;
    int minDifference = INT_MAX;

    for (int i = 0; i < staticArrayOfBlocks->sizeOfArray; i++)
    {

        for (int j = 0; j < staticArrayOfBlocks->sizeOfBlock; j++)
        {
            int position = (i * staticArrayOfBlocks->sizeOfBlock) + j;
            tmpSum += array[position];
        }

        int diff = abs(sum - tmpSum);

        if (diff < minDifference)
        {
            minDifference = diff;
            indexOfMinDiff = i;
        }
    }

    return indexOfMinDiff;
}
