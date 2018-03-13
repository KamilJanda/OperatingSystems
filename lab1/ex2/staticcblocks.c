//
// Created by Kamil on 11.03.2018.
//

#include "staticcblocks.h"

StaticstaticArrayOfBlocks *create_array_of_blocks_static(int sizeOfArray, int sizeOfBlock)
{
    StaticstaticArrayOfBlocks staticstaticArrayOfBlocks = malloc(sizeof(StaticstaticArrayOfBlocks));

    staticstaticArrayOfBlocks -> sizeOfArray = sizeOfArray;
    staticstaticArrayOfBlocks -> sizeOfBlock = sizeOfBlock;
    staticstaticArrayOfBlocks -> blockAvailabilityMap = calloc(sizeOfArray,sizeof(int));
}

void delete_array_of_blocks_static(StaticstaticArrayOfBlocks* staticstaticArrayOfBlocks,char array[])
{
    int size=(staticstaticArrayOfBlocks -> sizeOfArray)*(staticstaticArrayOfBlocks -> sizeOfBlock);

    for(int i=0;i<size;i++)
    {
        array[i]='';
    }

    free(staticstaticArrayOfBlocks);
}

void add_block_static(StaticstaticArrayOfBlocks *staticstaticArrayOfBlocks, int index, char block[],int blockSize, char array[])
{
    if(staticstaticArrayOfBlocks->blockAvailabilityMap[index]==1)
    {
        printf("Error: Block with given index is not empty");
        return;
    }
    if (index >= staticstaticArrayOfBlocks->sizeOfArray)
    {
        printf("Error: index is out of range");
        return;
    }
    if (index < 0)
    {
        printf("Error: index cannot be negative number");
        return;
    }
    if (staticstaticArrayOfBlocks == NULL)
    {
        printf("Error:  staticstaticArrayOfBlocks is NULL");
        return;
    }
    if(blockSize != staticstaticArrayOfBlocks->sizeOfBlock)
    {
        printf("Error: block size is incorrect");
        return;
    }

    staticstaticArrayOfBlocks->blockAvailabilityMap[index]=1

    int position;
    for(int i=0;i<staticstaticArrayOfBlocks;i++)
    {
        position = (index*staticstaticArrayOfBlocks->sizeOfBlock)+i
        array[position] = block[i];        
    }

}

void add_block_with_random_data_static(StaticstaticArrayOfBlocks *staticstaticArrayOfBlocks, int index,char array[])
{
    char* block = calloc(staticArrayOfBlocks->sizeOfBlock,sizeof(char));

    for (int i = 0; i < staticArrayOfBlocks->sizeOfBlock; i++)
    {
        int r = rand() % 25 + 97;
        block[i] = (char)r;
    }

    add_block_static(staticstaticArrayOfBlocks,i ndex, block, blockSize, array);
}

void delete_block_static(StaticstaticArrayOfBlocks *staticstaticArrayOfBlocks, int index, char array[])
{
    if (index >= staticstaticArrayOfBlocks->sizeOfArray)
    {
        printf("Error: index is out of range");
        return;
    }
    if (index < 0)
    {
        printf("Error: index cannot be negative number");
        return;
    }
    if (staticstaticArrayOfBlocks == NULL)
    {
        printf("Error: array of blocks is NULL");
        return;
    }
    
    staticstaticArrayOfBlocks->blockAvailabilityMap[index]=0

    int position;
    for(int i=0;i<staticstaticArrayOfBlocks;i++)
    {
        position = (index*staticstaticArrayOfBlocks->sizeOfBlock)+i
        array[position] = '';        
    }


}

int find_block_static(StaticstaticArrayOfBlocks *staticstaticArrayOfBlocks, int sum, char array[])
{
    int tmpSum = 0;
    int indexOfMinDiff = -1;
    int minDifference = INT_MAX;

    for (int i = 0; i < staticArrayOfBlocks->sizeOfArray; i++)
    {
        
        for(int j=0;j<staticstaticArrayOfBlocks->sizeOfBlock;j++)
        {
            int position = (i*staticstaticArrayOfBlocks->sizeOfBlock)+j;
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
