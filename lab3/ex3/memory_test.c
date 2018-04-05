#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[])
{   
    char** test = NULL;

    while(1)
    {
        test = calloc(10000,sizeof(char*));
	test = test;
    }


    return 0;
}
