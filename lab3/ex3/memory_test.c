#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{   
    char** test = NULL;

    while(1)
    {
        test = NULL;
        test = calloc(10000,sizeof(char*));
    }

    return 0;
}