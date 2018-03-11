//
// Created by Kamil on 11.03.2018.
//

#include "dynamicblocks.h"
#include <stdlib.h>


char** create_array_of_blocks(int sizeOfTable, int sizeOfBlock)
{
    char** newArrayOfBlocks = calloc(sizeOfTable, sizeof(char*));

    for(int i=0;i<sizeOfTable;i++)
    {
        newArrayOfBlocks[i] = calloc(sizeOfBlock, sizeof(char));
    }

    return newArrayOfBlocks;
}

void** delete_array_of_blocks()
{

}

char** add_block(char** array,int arraySize,int index,char* block)
{
    int newSizeOfArray = arraySize+1;
    char** newArray = calloc(newSizeOfArray, sizeof(char*));

    for(int i=0;i<index;i++)
    {
        newArray[i] = array[i];
    }

    newArray[index] = block;

    for (int i = index+1; i < newSizeOfArray; ++i) {
        newArray[i] = array[i];
    }

    free(array);

    return newArray;
}

char** delete_block(char** array,int arraySize,int index)
{
    int newSizeOfArray = arraySize-1;
    char** newArray = calloc(newSizeOfArray, sizeof(char*));

    for(int i=0;i<index;i++)
    {
        newArray[i] = array[i];
    }

    for (int i = index+1; i < newSizeOfArray; ++i) {
        newArray[i] = array[i];
    }

    free(array);

    return newArray;
}

