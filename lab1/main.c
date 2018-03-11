#include <stdio.h>
#include <stdlib.h>
#include "dynamicblocks.h"

void createArray();

int main( int argc, char *argv[])  {

    if(argc<5)
    {
        printf("Too few arguments\n");
        printf("example call: ./main [size of array] [size of blocks] [d/s] [operations]\n");
        exit(0);
    }

    int arraySize = (int)argv[1];
    int blockSize = (int)argv[2];
    char allocationType = argv[3];

    



    ArrayOfBlocks* test = create_array_of_blocks(2,3);


    printf("%d",test-> array[1][2]);
    return 0;
}

void createArray()
{

}