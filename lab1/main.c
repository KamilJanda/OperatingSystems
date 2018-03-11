#include <stdio.h>
#include "dynamicblocks.h"

int main() {

    ArrayOfBlocks* test = create_array_of_blocks(2,3);


    printf("%d",test-> array[1][2]);
    return 0;
}