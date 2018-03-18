#include "fileoperations.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


FileData* create_file_data(char *fileName,int numberOfRecords,int sizeOfRecord)
{
    FileData* result = malloc(sizeof(FileData));
    result->fileName = fileName;
    result->numberOfRecords = numberOfRecords;
    result->sizeOfRecord = sizeOfRecord;

    return result;
}

void generate(FileData fileData)
{
    char* accessMode = "w";
    FILE* file = fopen(fileData.fileName,accessMode);

    char* randomArray = create_random_array_of_records(fileData.numberOfRecords, fileData.sizeOfRecord);

    fwrite(randomArray,sizeof(randomArray[0]),fileData.numberOfRecords*fileData.sizeOfRecord,file);

    fclose(file);
}

void sort_sys(FileData fileData)
{
    



}

void sort_lib(FileData fileData)
{

}

void copy_sys(FileData* fileDataOrginal,FileData* fileDataCopy, int numberOfRecordToCopy)
{
    fileDataCopy->numberOfRecords = numberOfRecordToCopy;
    fileDataCopy->sizeOfRecord = fileDataOrginal->sizeOfRecord;

    int fileOrginal = open(fileDataOrginal->fileName,O_RDONLY);
    int fileCopy = open(fileDataCopy->fileName,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);

    if(fileOrginal<0 || fileCopy<0)
    {
        char* errorFile = fileOrginal<0 ? fileOrginal : fileCopy;
        printf("Error: cannot open file: \"%s\"\n",fileDataOrginal->fileName);
        exit(1);
    }

    int cnt = 0;

    char* buff = calloc(fileDataOrginal->sizeOfRecord,sizeof(char));

    while(((cnt=read(fileOrginal,buff,fileDataOrginal->sizeOfRecord))>0) && numberOfRecordToCopy>0)
    {
        write(fileCopy,buff,cnt);   
        numberOfRecordToCopy--;  
    }

    close(fileOrginal);
    close(fileCopy);
}

void copy_lib(FileData* fileDataOrginal,FileData* fileDataCopy, int numberOfRecordToCopy)
{

}

char* create_random_array_of_records(int numberOfRecords, int sizeOfRecord)
{
    char* result = calloc(numberOfRecords*sizeOfRecord,sizeof(char));

    srand(time(NULL));
    for(int i=0;i<numberOfRecords*sizeOfRecord;i++)
    {
        result[i] = rand()%26 + 'a';
    }

    return result;
}


