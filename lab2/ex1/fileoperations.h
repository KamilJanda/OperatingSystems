#ifndef LAB2_FILEOPERATIONS_H
#define LAB2_FILEOPERATIONS_H

typedef struct
{
    char *fileName;
    int numberOfRecords;
    int sizeOfRecord;

}FileData;

FileData* create_file_data(char *fileName,int numberOfRecords,int sizeOfRecord);
void generate(FileData fileData);
void sort_lib(FileData fileData);
void copy_sys(FileData* fileDataOrginal,FileData* fileDataCopy, int numberOfRecordToCopy);
void copy_lib(FileData* fileDataOrginal,FileData* fileDataCopy, int numberOfRecordToCopy);
char* create_random_array_of_records(int numberOfRecords, int sizeOfRecord);


#endif