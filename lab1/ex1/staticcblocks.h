//
// Created by Kamil on 11.03.2018.
//

#ifndef LAB1_STATICCBLOCKS_H
#define LAB1_STATICCBLOCKS_H

typedef struct
{
    int sizeOfArray;
    int sizeOfBlock;
    int* arrayOfUsed;
} StaticArrayOfBlocks;

StaticArrayOfBlocks *create_array_of_blocks(int sizeOfArray, int sizeOfBlock);

void delete_array_of_blocks(StaticArrayOfBlocks* arrayOfBlocks);

void add_block(StaticArrayOfBlocks *arrayOfBlocks, int index, char *block,int blockSize);

void add_block_with_random_data(StaticArrayOfBlocks *arrayOfBlocks, int index);

void delete_block(StaticArrayOfBlocks *arrayOfBlocks, int index);

char *find_block(StaticArrayOfBlocks *arrayOfBlocks, int sum);

int sum_of_block(char *block, int size);


#endif //LAB1_STATICCBLOCKS_H
