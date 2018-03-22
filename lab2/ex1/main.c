#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "fileoperations.h"
#include "timeutil.h"

struct Results
{
    double *generateTimes;
    double *sortSysTimes;
    double *sortLibTimes;
    double *copySysTimes;
    double *copyLibTimes;
};

void parser(int argc, char *argv[]);
int check_nuber_of_arguments(char *operation, int numberOfArguments);
struct Results* init_results();
void print_results(struct Results* results, char *operation,char *type);

int main(int argc, char *argv[])
{
    parser(argc, argv);

/*
    int fd = open("dane",O_RDWR);

    char* buff1 = calloc(4,sizeof(char));
    char* buff2 = calloc(4,sizeof(char));

    if(read(fd,buff1,4)<0) printf("error buff1 \n");
    if(read(fd,buff2,4)<0) printf("error buff1 \n");

    swap_in_file_sys(buff1,buff2,fd,4);

    lseek(fd,0,SEEK_SET);

    if(read(fd,buff1,4)<0) printf("error buff1 \n");

    printf("%s \n",buff1);

    close(fd);

*/

    return 0;
}

void parser(int argc, char *argv[])
{
    char generateSymbol[] = "generate";
    char copy[] = "copy";
    char sort[] = "sort";
    char* type = NULL;

    struct Results* results= init_results();

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



    if(strcmp(operation,generateSymbol) == 0 && check_nuber_of_arguments(operation,argc) == 0)
    {
        FileData fileData;
        fileData.fileName = argv[2];
        fileData.numberOfRecords = atoi(argv[3]);
        fileData.sizeOfRecord = atoi(argv[4]);

        start_clock();

        generate(fileData);

        end_clock(results->generateTimes);

    }
    else if (strcmp(operation,sort)==0 && check_nuber_of_arguments(operation,argc) == 0)
    {
        FileData* fileData = create_file_data(argv[2],atoi(argv[3]),atoi(argv[4]));


        if(strcmp(argv[5],"sys")==0)
        {
            type = argv[5];
            start_clock();
            sort_sys(fileData);
            end_clock(results->sortSysTimes);
        }
        else if(strcmp(argv[5],"lib")==0)
        {
            type = argv[5];
            start_clock();
            sort_lib(fileData);
            end_clock(results->sortLibTimes);
        }
        else
        {
            printf("Exception: Invalid last argument");
            exit(1);
        }

    }
    else if (strcmp(argv[1],copy)==0 && check_nuber_of_arguments(operation,argc) == 0)
    {
        FileData* fileDataOrginal = create_file_data(argv[2],atoi(argv[4]),atoi(argv[5]));
        FileData* fileDataCopy = create_file_data(argv[3],atoi(argv[4]),atoi(argv[5]));

        int numberOfRecordToCopy = atoi(argv[4]);

        if(strcmp(argv[6],"sys")==0)
        {
            type = argv[6];
            start_clock();
            copy_sys(fileDataOrginal,fileDataCopy,numberOfRecordToCopy);
            end_clock(results->copySysTimes);
        }
        else if(strcmp(argv[6],"lib")==0)
        {
            type = argv[6];
            start_clock();
            copy_lib(fileDataOrginal,fileDataCopy,numberOfRecordToCopy);
            end_clock(results->copyLibTimes);
        }
        else
        {
            printf("Exception: Invalid last argument");
            exit(1);
        }
    }


    print_results(results,operation,type);
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


struct Results* init_results()
{
    struct Results *results = malloc(sizeof(struct Results));

    results -> generateTimes = calloc(3,sizeof(double));
    results -> sortSysTimes = calloc(3,sizeof(double));
    results -> sortLibTimes = calloc(3,sizeof(double));
    results -> copySysTimes = calloc(3,sizeof(double));
    results -> copyLibTimes = calloc(3,sizeof(double));
    return results;
}

void print_results(struct Results* results, char *operation,char *type)
{
    if(strcmp(operation,"generate")==0)
    {
        printf("Create times: \n");
        printf("Real Time: %f, User Time %f, System Time %f \n\n", results->generateTimes[0], results->generateTimes[1], results->generateTimes[2]);
    }
    else if (strcmp(operation,"sort")==0 && strcmp(type,"sys") == 0)
    {
        printf("Sort sys times: \n");
        printf("Real Time: %f, User Time %f, System Time %f \n\n", results->sortSysTimes[0], results->sortSysTimes[1], results->sortSysTimes[2]);
    }
    else if (strcmp(operation,"sort")==0 && strcmp(type,"lib") == 0)
    {
        printf("Sort lib times: \n");
        printf("Real Time: %f, User Time %f, System Time %f \n\n", results->sortLibTimes[0], results->sortLibTimes[1], results->sortLibTimes[2]); 
    }
    else if (strcmp(operation,"copy")==0 && strcmp(type,"sys") == 0)
    {
        printf("Copy sys times: \n");
        printf("Real Time: %f, User Time %f, System Time %f \n\n", results->copySysTimes[0], results->copySysTimes[1], results->copySysTimes[2]); 
    }
    else if (strcmp(operation,"copy")==0 && strcmp(type,"lib") == 0)
    {
        printf("Copy lib times: \n");
        printf("Real Time: %f, User Time %f, System Time %f \n\n", results->copyLibTimes[0], results->copyLibTimes[1], results->copyLibTimes[2]);
    }

}