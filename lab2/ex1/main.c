#include <stdio.h>
#include <stdlib.h>

void parser(int argc, char *argv[]);
int check_nuber_of_arguments(char *operation, int numberOfArguments);

int main(int argc, char *argv[])
{
    parser(argc, argv);

    return 0;
}

void parser(int argc, char *argv[])
{
    char generate[] = "generate";
    char copy[] = "copy";
    char sort[] = "sort";


    if(argc<2)
    {
        printf("Exception: Too few arguments \n");
        return;
    }

    char *operation = argv[1];

    if (!((strcmp(operation,generate)==0)||(strcmp(operation,copy)==0)||(strcmp(sort,sort)==0)))
    {
        printf("Exception: Unknown operation \n");
        return;
    }



    if(strcmp(argv[1],generate) == 0 && check_nuber_of_arguments(operation,argc) == 0)
    {
        


    }
    else if (strcmp(argv[1],sort)==0 && check_nuber_of_arguments(operation,argc) == 0)
    {
        

    }
    else if (strcmp(argv[1],copy)==0 && check_nuber_of_arguments(operation,argc) == 0)
    {

    }



}

//return 1 for invalid number of arguments, 0 otherwise
int check_nuber_of_arguments(char *operation, int numberOfArguments)
{
    int numberOfArgForGnereate = 5;
    int numberOfArgForSort = 6;
    int numberOfArgForCopy = 7;

    char message[] = "Exception: invalid number of arguments after:";

    if(numberOfArguments != numberOfArgForGnereate)
    {
        printf("%s %s \n",message,operation);
        return 1;
    }
    else if(numberOfArguments != numberOfArgForSort)
    {
        printf("%s %s \n",message,operation);
        return 1;
    }
    else if(numberOfArguments != numberOfArgForCopy)
    {
        printf("%s %s \n",message,operation);
        return 1;
    }
    else
    {
        return 0;
    }
}