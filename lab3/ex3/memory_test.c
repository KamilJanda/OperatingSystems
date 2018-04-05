#include <stdio.h>
#include <stdlib.h>

void message()
{
    printf("hi \n");
}

int main(int argc, char *argv[])
{   
    char** test = NULL;

    while(1)
    {
        test = NULL;
        test = calloc(10000,sizeof(char*));
    }

    //atexit(message);

    return 0;
}