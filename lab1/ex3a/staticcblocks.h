//
// Created by Kamil on 11.03.2018.
//

#ifndef LAB1_STATICCBLOCKS_H
#define LAB1_STATICCBLOCKS_H

typedef struct
{
    int sizeOfArray;
    int sizeOfBlock;
    int* blockAvailabilityMap;
} StaticArrayOfBlocks;

StaticArrayOfBlocks *create_array_of_blocks_static(int sizeOfArray, int sizeOfBlock);

void deleteArrayOfBlocksStatic(StaticArrayOfBlocks* staticArrayOfBlocks,char array[]);

void add_block_static(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char block[],int blockSize, char array[]);

void addBlockWithRandomDataStatic(StaticArrayOfBlocks *staticArrayOfBlocks, int index,char array[]);

void delete_block_static(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char array[]);

int find_block_static(StaticArrayOfBlocks *staticArrayOfBlocks, int sum, char array[]);


#endif //LAB1_STATICCBLOCKS_H
