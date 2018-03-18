#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileoperations.h"

void parser(int argc, char *argv[]);
int check_nuber_of_arguments(char *operation, int numberOfArguments);

int main(int argc, char *argv[])
{
    parser(argc, argv);

    return 0;
}

void parser(int argc, char *argv[])
{
    char generateSymbol[] = "generate";
    char copy[] = "copy";
    char sort[] = "sort";


    if(argc<2)
    {
        printf("Exception: Too few arguments \n");
        exit(1);
    }

    char *operation = argv[1];

    if (!((strcmp(operation,generateSymbol)==0)||(strcmp(operation,copy)==0)||(strcmp(sort,sort)==0)))
    {
        printf("Exception: Unknown operation \n");
        exit(1);
    }



    if(strcmp(argv[1],generateSymbol) == 0 && check_nuber_of_arguments(operation,argc) == 0)
    {
        FileData fileData;
        fileData.fileName = argv[2];
        fileData.numberOfRecords = atoi(argv[3]);
        fileData.sizeOfRecord = atoi(argv[4]);

        generate(fileData);

    }
    else if (strcmp(argv[1],sort)==0 && check_nuber_of_arguments(operation,argc) == 0)
    {
        

    }
    else if (strcmp(argv[1],copy)==0 && check_nuber_of_arguments(operation,argc) == 0)
    {
        FileData* fileDataOrginal = create_file_data(argv[2],atoi(argv[4]),atoi(argv[5]));
        FileData* fileDataCopy = create_file_data(argv[3],atoi(argv[4]),atoi(argv[5]));

        int numberOfRecordToCopy = atoi(argv[4]);

        if(strcmp(argv[6],"sys")==0)
        {
            copy_sys(fileDataOrginal,fileDataCopy,numberOfRecordToCopy);
        }
        else if(strcmp(argv[6],"lib")==0)
        {
            
        }
        else
        {
            printf("Exception: Invalid last argument");
            exit(1);
        }
    }



}

//return 1 for invalid number of arguments, 0 otherwise
int check_nuber_of_arguments(char *operation, int numberOfArguments)
{
    int numberOfArgForGnereate = 5;
    int numberOfArgForSort = 6;
    int numberOfArgForCopy = 7;

    char message[] = "Exception: invalid number of arguments after:";

    if((strcmp(operation,"generateSymbol")==0)&&(numberOfArguments != numberOfArgForGnereate))
    {
        printf("%s %s \n",message,operation);
        return 1;
    }
    else if((strcmp(operation,"sort")==0)&&(numberOfArguments != numberOfArgForSort))
    {
        printf("%s %s \n",message,operation);
        return 1;
    }
    else if((strcmp(operation,"copy")==0)&&(numberOfArguments != numberOfArgForCopy))
    {
        printf("%s %s \n",message,operation);
        return 1;
    }
    else
    {
        return 0;
    }
}

