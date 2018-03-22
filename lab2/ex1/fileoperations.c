#include "fileoperations.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FREE(p)   do { free(p); (p) = NULL; } while(0)

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

void sort_sys(FileData *fileData)
{
    int fd = open(fileData->fileName,O_RDWR);

    char* buff1 = calloc(fileData->sizeOfRecord,sizeof(char));
    char* buff2 = calloc(fileData->sizeOfRecord,sizeof(char));

    int cnt = fileData->numberOfRecords;

    int j;
    for(int i=1; i<cnt; i++)
    {
        j=i;

        if(read(fd,buff1,fileData->sizeOfRecord)<0) printf("error buff1 \n");

        if(read(fd,buff2,fileData->sizeOfRecord)<0) printf("error buff2 \n");

        //count number of step back in order to restore correct position in file after loop
        int numberOfMoveBackInFile = 0;
        while(j>0 && compare(buff1,buff2) == 1)
        {
            swap_in_file_sys(buff1,buff2,fd,fileData->sizeOfRecord);
            //printf("swap: buff1 = %s buff2 = %s \n",buff1,buff2);
            j--;
            if(j>0){ 
                //one step back in file 
                lseek(fd,(off_t)(fileData->sizeOfRecord*(-1)),SEEK_CUR);
                numberOfMoveBackInFile++;
                //printf("j = %d buff2 = %s \n",j,buff2);
                buff1 = read_n_previous_record_sys(2,fd,fileData->sizeOfRecord);
                //printf("j = %d buff1 = %s \n",j,buff1);
            }
        }

        //restore correct position in file after loop
        lseek(fd,(off_t)((fileData->sizeOfRecord)*(numberOfMoveBackInFile)),SEEK_CUR);

        //one step back in file, because we used read two times
        lseek(fd,(off_t)(fileData->sizeOfRecord*(-1)),SEEK_CUR);
    }

    close(fd);
    FREE(buff1);
    FREE(buff1);

}

void sort_lib(FileData *fileData)
{
    FILE *fptr = fopen(fileData->fileName,"r+");

    char* buff1 = calloc(fileData->sizeOfRecord,sizeof(char));
    char* buff2 = calloc(fileData->sizeOfRecord,sizeof(char));

    int cnt = fileData->numberOfRecords;

    int j;
    for(int i=1; i<cnt; i++)
    {
        j=i;

        if(fread(buff1,sizeof(char),fileData->sizeOfRecord,fptr) == 0) printf("error buff1 \n");

        if(fread(buff2,sizeof(char),fileData->sizeOfRecord,fptr) == 0) printf("error buff2 \n");

        //count number of step back in order to restore correct position in file after loop
        int numberOfMoveBackInFile = 0;
        while(j>0 && compare(buff1,buff2) == 1)
        {
            swap_in_file_lib(buff1,buff2,fptr,fileData->sizeOfRecord);
            //printf("swap: buff1 = %s buff2 = %s \n",buff1,buff2);
            j--;
            if(j>0){ 
                //one step back in file 
                fseek(fptr,(long)(fileData->sizeOfRecord*(-1)),SEEK_CUR);
                numberOfMoveBackInFile++;
                //printf("j = %d buff2 = %s \n",j,buff2);
                buff1 = read_n_previous_record_lib(2,fptr,fileData->sizeOfRecord);
                //printf("j = %d buff1 = %s \n",j,buff1);
            }
        }

        //restore correct position in file after loop
        fseek(fptr,(long)((fileData->sizeOfRecord)*(numberOfMoveBackInFile)),SEEK_CUR);

        //one step back in file, because we used read two times
        fseek(fptr,(long)(fileData->sizeOfRecord*(-1)),SEEK_CUR);
    }

    fclose(fptr);
    FREE(buff1);
    FREE(buff1);
}

void copy_sys(FileData* fileDataOrginal,FileData* fileDataCopy, int numberOfRecordToCopy)
{
    fileDataCopy->numberOfRecords = numberOfRecordToCopy;
    fileDataCopy->sizeOfRecord = fileDataOrginal->sizeOfRecord;

    int fileOrginal = open(fileDataOrginal->fileName,O_RDONLY);
    int fileCopy = open(fileDataCopy->fileName,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);

    if(fileOrginal<0 || fileCopy<0)
    {
        char* errorFile = fileOrginal < 0 ? fileDataOrginal->fileName : fileDataCopy->fileName;
        printf("Error: cannot open file: \"%s\"\n",errorFile);
        exit(1);
    }

    int cnt = 0;

    char* buff = calloc(fileDataOrginal->sizeOfRecord,sizeof(char));

    while(((cnt=read(fileOrginal,buff,fileDataOrginal->sizeOfRecord))>0) && numberOfRecordToCopy>0)
    {
        write(fileCopy,buff,cnt);   
        numberOfRecordToCopy--;  
    }

    free(buff);
    close(fileOrginal);
    close(fileCopy);
}

void copy_lib(FileData* fileDataOrginal,FileData* fileDataCopy, int numberOfRecordToCopy)
{
    fileDataCopy->numberOfRecords = numberOfRecordToCopy;
    fileDataCopy->sizeOfRecord = fileDataOrginal->sizeOfRecord;

    FILE *fileOrginalPtr = fopen(fileDataOrginal->fileName,"r");
    FILE *fileCopyPtr = fopen(fileDataCopy->fileName,"w");

    if(fileOrginalPtr == NULL || fileCopyPtr == NULL)
    {
        char* errorFile = fileOrginalPtr == NULL ? fileDataOrginal->fileName : fileDataCopy->fileName;
        printf("Error: cannot open file: \"%s\"\n",errorFile);
        exit(1);
    }

    int cnt = 0;

    char* buff = calloc(fileDataOrginal->sizeOfRecord,sizeof(char));

    while((cnt=fread(buff,sizeof(char),fileDataOrginal->sizeOfRecord,fileOrginalPtr))>0 && numberOfRecordToCopy>0)
    {
        fwrite(buff,sizeof(char),cnt,fileCopyPtr);   
        numberOfRecordToCopy--;  
    }

    free(buff);
    fclose(fileOrginalPtr);
    fclose(fileCopyPtr);
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

int compare(char* val1, char* val2)
{
    unsigned char fchar1 =(unsigned char) val1[0];
    unsigned char fchar2 =(unsigned char) val2[0];

    return fchar1 > fchar2 ? 1 : fchar1 == fchar2 ? 0 : -1 ;
}

void swap_in_file_sys(char *buff1,char *buff2,int fd, int recordSize)
{
    lseek(fd, (off_t)(recordSize*(-2)),SEEK_CUR);
    write(fd,buff2,recordSize);
    write(fd,buff1,recordSize);
}

char* read_n_previous_record_sys(int n,int fd,int recordSize)
{
    char* buff = calloc(recordSize,sizeof(char));

    lseek(fd, (off_t)(recordSize*(-n)),SEEK_CUR);
    read(fd,buff,recordSize);
    lseek(fd, (off_t)(recordSize*(n-1)),SEEK_CUR); 

    return buff;
}

void swap_in_file_lib(char *buff1,char *buff2,FILE *fptr, int recordSize)
{
    fseek(fptr, (long)(recordSize*(-2)),SEEK_CUR);
    fwrite(buff2,sizeof(char),recordSize,fptr);
    fwrite(buff1,sizeof(char),recordSize,fptr);
}

char* read_n_previous_record_lib(int n,FILE *fptr,int recordSize)
{
    char* buff = calloc(recordSize,sizeof(char));

    fseek(fptr, (long)(recordSize*(-n)),SEEK_CUR);
    fread(buff,sizeof(char),recordSize,fptr);
    fseek(fptr, (long)(recordSize*(n-1)),SEEK_CUR); 

    return buff;
}