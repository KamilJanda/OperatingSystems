#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

int EXTENDED_PRINT = 0;

typedef struct Monitor Monitor;

struct Monitor
{
    
    int bufferSize;
    char **buffer;
    int count;
    int writeIndex;
    int readIndex;
    int producersNumber;
    int consumentsNumber;
    int L;
    char *printMode;
    char *searchMode;
    int nk;
    int full;
    FILE *file;

    sem_t  read;
    sem_t  write;
    sem_t  string_cmp;

    char comparator;
};


void parse(int argc, char *argv[])
{
    if (argc != 2)
        perror("wrong number of arguments\n");
}

void read_config_file(
    char *configFileName,
    int *producersNumber,
    int *consumentsNumber,
    int *bufferSize,
    char *fileName,
    int *L,
    char *searchMode,
    char *printMode,
    int *nk)
{
    FILE *fp;

    if ((fp = fopen(configFileName, "r")) == NULL)
        perror("failed to read file");

    fscanf(fp, "%d %d %d %s %d %s %s %d", producersNumber, consumentsNumber, bufferSize, fileName, L, searchMode, printMode, nk);
    fclose(fp);
}

Monitor *init_monitor(char *path)
{
    Monitor *monitor = malloc(sizeof(Monitor));

    FILE *fp;
    char fileName[64];
    int printMode;

    if ((fp = fopen(path, "r")) == NULL)
        perror("failed to read file");

    fscanf(
        fp,
        "%d %d %d %s %d %s %d %d",
        &(monitor->producersNumber),
        &(monitor->consumentsNumber),
        &(monitor->bufferSize),
        fileName,
        &(monitor->L),
        &(monitor->comparator),
        &printMode,
        &(monitor->nk));

    fclose(fp);

    EXTENDED_PRINT = printMode;

    monitor->buffer = calloc(monitor->bufferSize, sizeof(char *));
    for (int i = 0; i < monitor->bufferSize; ++i)
        monitor->buffer[i] = NULL;

    sem_init(&(monitor->read), 0, 0);
    sem_init(&(monitor->write), 0, monitor->bufferSize);
    sem_init(&(monitor->string_cmp), 0, 1);

    monitor->readIndex = 0;
    monitor->writeIndex = 0;
    monitor->full = 0;

    monitor->file = fopen(fileName, "r");

    return monitor;
}

char *produce_item(Monitor *monitor)
{
    char* line = NULL;
    size_t size = 0;
    if ((getline(&line, &size, monitor->file)) < 0) {
        return "eof";
    }
    return line;
}

void consume_item(int L,char comp,char *item)
{
    int len = strlen(item);

    if(comp == '<' && len < L)
    {
        printf("Consumed: %s\n", item);
    }
    else if (comp == '=' && len == L)
    {
        printf("Consumed: %s\n", item);
    }
    else if (comp == '>' && len > L)
    {
        printf("Consumed: %s", item);
    }

    
}

// --Producer and consumer--



int produce(Monitor *this)
{
    sem_wait(&(this->write));
    sem_wait(&(this->string_cmp));

    if(EXTENDED_PRINT) printf("Producer get lock \n");

    char *item = produce_item(this);

    if(strcmp("eof", item) == 0) return 0;


    if(EXTENDED_PRINT) printf("Producer write %s \n",item);
    this->buffer[this->writeIndex] = item;

    if ((this->writeIndex + 1) % this->bufferSize == this->readIndex) this->full = 1;

    

    this->writeIndex = (this->writeIndex + 1) % this->bufferSize;
    this->count++;


    if(EXTENDED_PRINT) printf("Producer unlock sem\n");

    sem_post(&(this->string_cmp));
    sem_post(&(this->read));

    return 1;
}

void *producer(void *ptr)
{
    Monitor *monitor = (Monitor *)(ptr);
    while (produce(monitor));
        
    return 0;
}

void consume(Monitor *this)
{
    sem_wait(&(this->read));
    sem_wait(&(this->string_cmp));


    if(EXTENDED_PRINT) printf("Consumer get lock \n");

    char *item = this->buffer[this->readIndex];

    consume_item(this->L,this->comparator,item);

    //free(this->buffer[this->writeIndex]);
    this->buffer[this->writeIndex] = NULL;

    this->readIndex = (this->readIndex + 1) % this->bufferSize;
    this->count--;

    sem_post(&(this->string_cmp));
    sem_post(&(this->write));
}

void *consumer(void *ptr)
{
    Monitor *monitor = (Monitor *)(ptr);
    while (1)
        consume(monitor);
}

void destroy_sem(Monitor *monitor) {
    sem_destroy(&(monitor->write));
    sem_destroy(&(monitor->read));
    sem_destroy(&(monitor->string_cmp));
}

// --- MAIN ---

int main(int argc, char *argv[])
{
    parse(argc, argv);

    char *configFileName = argv[1];

    Monitor *monitor = init_monitor(configFileName);

    pthread_t *thr = malloc(sizeof(pthread_t) * (monitor->producersNumber + monitor->consumentsNumber));
    for (int i = 0; i < monitor->producersNumber; i++)
    {
        pthread_create(&(thr[i]), NULL, producer, monitor);
    }
    for (int i = 0; i < monitor->consumentsNumber; i++)
    {
        pthread_create(&(thr[monitor->producersNumber + i]), NULL, consumer, monitor);
    }

    sleep(monitor->nk);

    printf("ending proceses'\n\n");

    for (int i = 0; i < (monitor->producersNumber + monitor->consumentsNumber); i++)
    {
        pthread_cancel(thr[i]);
    }

    destroy_sem(monitor);
    free(thr);

    return 0;
}