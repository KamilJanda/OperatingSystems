#ifndef LAB2_FILEOPERATIONS_H
#define LAB2_FILEOPERATIONS_H

#include <stdio.h>

typedef struct
{
    char *fileName;
    int numberOfRecords;
    int sizeOfRecord;

}FileData;

FileData* create_file_data(char *fileName,int numberOfRecords,int sizeOfRecord);
void generate(FileData fileData);
void sort_sys(FileData *fileData);
void sort_lib(FileData *fileData);
void copy_sys(FileData* fileDataOrginal,FileData* fileDataCopy, int numberOfRecordToCopy);
void copy_lib(FileData* fileDataOrginal,FileData* fileDataCopy, int numberOfRecordToCopy);
char* create_random_array_of_records(int numberOfRecords, int sizeOfRecord);
int compare(char* val1, char* val2);
void swap_in_file_sys(char *buff1,char *buff2,int fd, int recordSize);
char* read_n_previous_record_sys(int n,int fd,int recordSize);
void swap_in_file_lib(char *buff1,char *buff2,FILE *fptr, int recordSize);
char* read_n_previous_record_lib(int n,FILE *fptr,int recordSize);


#endif