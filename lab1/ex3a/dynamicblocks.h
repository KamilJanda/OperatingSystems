#ifndef LAB1_DYNAMICBLOCKS_H
#define LAB1_DYNAMICBLOCKS_H

typedef struct
{
    char **array;
    int sizeOfArray;
    int sizeOfBlock;
} ArrayOfBlocks;

ArrayOfBlocks *create_array_of_blocks(int sizeOfArray, int sizeOfBlock);

void delete_array_of_blocks(ArrayOfBlocks* arrayOfBlocks);

void add_block(ArrayOfBlocks *arrayOfBlocks, int index, char *block,int blockSize);

void add_block_with_random_data(ArrayOfBlocks *arrayOfBlocks, int index);

void delete_block(ArrayOfBlocks *arrayOfBlocks, int index);

char *find_block(ArrayOfBlocks *arrayOfBlocks, int sum);

int sum_of_block(char *block, int size);


#endif