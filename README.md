# OperatingSystems
Programming projects in an Operating Systems Course

#ifndef LAB1_DYNAMICBLOCKS_H
#define LAB1_DYNAMICBLOCKS_H

char** create_array_of_blocks(int sizeOfTable, int sizeOfBlock);
void** delete_array_of_blocks();
char** add_block(char** array,int arraySize,int index,char* block);
char** delete_block(char** array,int arraySize,int index);


#endif
