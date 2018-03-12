//
// Created by Kamil on 11.03.2018.
//

#include "dynamicblocks.h"
#include <stdlib.h>
#include <math.h>

ArrayOfBlocks *create_array_of_blocks(int sizeOfArray, int sizeOfBlock)
{
    ArrayOfBlocks *newArrayOfBlocks = malloc(sizeof(ArrayOfBlocks));

    newArrayOfBlocks->sizeOfArray = sizeOfArray;
    newArrayOfBlocks->sizeOfBlock = sizeOfBlock;
    newArrayOfBlocks->array = calloc(sizeOfArray, sizeof(char *));

    srand(time(NULL));

    for (int i = 0; i < sizeOfArray; i++)
    {
        newArrayOfBlocks->array[i] = calloc(sizeOfBlock, sizeof(char));

        for (int j = 0; j < newArrayOfBlocks->sizeOfBlock; j++)
        {
            int r = rand() % 25 + 97;
            newArrayOfBlocks->array[i][j] = (char)r;
        }
    }

    return newArrayOfBlocks;
}

void **delete_array_of_blocks(char **array)
{
    free(array);
}

ArrayOfBlocks *add_block(ArrayOfBlocks *arrayOfBlocks, int index, char *block)
{
    int newSizeOfArray = arrayOfBlocks->sizeOfArray + 1;
    char **newArray = calloc(newSizeOfArray, sizeof(char *));

    for (int i = 0; i < index; i++)
    {
        newArray[i] = arrayOfBlocks->array[i];
    }

    newArray[index] = block;

    for (int i = index + 1; i < newSizeOfArray; ++i)
    {
        newArray[i] = arrayOfBlocks->array[i];
    }

    free(arrayOfBlocks->array);

    arrayOfBlocks->array = newArray;
    return arrayOfBlocks;
}

ArrayOfBlocks *delete_block(ArrayOfBlocks *arrayOfBlocks, int index)
{
    int newSizeOfArray = arrayOfBlocks->sizeOfArray - 1;
    char **newArray = calloc(newSizeOfArray, sizeof(char *));

    for (int i = 0; i < index; i++)
    {
        newArray[i] = arrayOfBlocks->array[i];
    }

    for (int i = index + 1; i < newSizeOfArray; ++i)
    {
        newArray[i] = arrayOfBlocks->array[i];
    }

    free(arrayOfBlocks->array);
    arrayOfBlocks->array = newArray;
    return arrayOfBlocks;
}

char *find_block(ArrayOfBlocks *arrayOfBlocks, int sum)
{
    int tmpSum = 0;
    int indexOfMinDiff = -1;
    int minDifference = INT16_MAX;

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

int sum_of_block(char *block, int size)
{
    int sum = 0;
    for (int i = 0; i < size; i++)
    {
        if (block[i] == '\0')
            return sum;

        sum += (int)block[i];
    }
    return sum;
}

void fill_in_with_random_data(ArrayOfBlocks *arrayOfBlocks)
{

    srand(time(NULL));

    for (int i = 0; arrayOfBlocks->sizeOfArray; i++)
    {

        for (int j = 0; j < arrayOfBlocks->sizeOfBlock; j++)
        {
            int r = rand() % 25 + 97;
            arrayOfBlocks->array[i][j] = (char)r;
        }
    }
}