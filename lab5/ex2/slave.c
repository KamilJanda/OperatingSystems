#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 128

int parse(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Too few agruments\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 3)
    {
        printf("Too many agruments\n");
        exit(EXIT_FAILURE);
    }

    return 1;
}

int main(int argc, char *argv[])
{
    parse(argc, argv);

    char *path = argv[1];
    int numberOfOperation = atoi(argv[2]);

    FILE *fifo = fopen(path, "w");
    if (fifo == NULL)
    {
        perror("Slave: Fail to open FIFO\n");
        exit(EXIT_FAILURE);
    }

    printf("pid: %d\n",getpid());

    
    for(int i=0;i<numberOfOperation;i++)
    {
        char buff[126];

        FILE *date = popen("date","r");
        if(date == NULL)
        {
            perror("Failed to open date");
            exit(EXIT_FAILURE);
        }

        char dateString[BUFFER_SIZE];
        fgets(dateString,BUFFER_SIZE,date);
        
        sprintf(buff, "<%d> %s",getpid(),dateString);
        fwrite(buff,sizeof(char),strlen(buff),fifo);
        fclose(date);
        sleep(rand()%5);
    }

    fclose(fifo);
    return 0;
}