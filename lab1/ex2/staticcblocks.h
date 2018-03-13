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

void delete_array_of_blocks_static(StaticArrayOfBlocks* staticArrayOfBlocks);

void add_block_static(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char block[],int blockSize, char array[]);

void add_block_with_random_data_static(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char array[]);

void delete_block_static(StaticArrayOfBlocks *staticArrayOfBlocks, int index, char array[]);

char[] find_block_static(StaticArrayOfBlocks *staticArrayOfBlocks, int sum, char array[]);

int find_block_static(StaticstaticArrayOfBlocks *staticstaticArrayOfBlocks, int sum, char array[]);

#endif //LAB1_STATICCBLOCKS_H
