#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

int EXTENDED_PRINT = 0;

typedef struct Monitor Monitor;

struct Monitor
{
    pthread_mutex_t mutex;
    int bufferSize;
    char **buffer;
    int count;
    pthread_cond_t cond;
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

    monitor->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    monitor->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

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
        return "eof\n";
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



void produce(Monitor *this)
{
    pthread_mutex_lock(&this->mutex);

    if(EXTENDED_PRINT) printf("Producer get lock \n");

    char *item = produce_item(this);

    //if (this->count == this->bufferSize)
    while (this->full == 1)
    {
        //if(EXTENDED_PRINT) printf("Producer go to sleep \n");
        pthread_cond_wait(&(this->cond), &(this->mutex));
    }

    if(EXTENDED_PRINT) printf("Producer write %s \n",item);
    this->buffer[this->writeIndex] = item;

    if ((this->writeIndex + 1) % this->bufferSize == this->readIndex) this->full = 1;

    if (this->writeIndex == this->readIndex)
    {
        if(EXTENDED_PRINT) printf("Producer wake up consumer \n");
        pthread_cond_broadcast(&(this->cond));
    }

    this->writeIndex = (this->writeIndex + 1) % this->bufferSize;
    this->count++;


    if(EXTENDED_PRINT) printf("Producer unlock mutex\n");
    pthread_mutex_unlock(&(this->mutex));
}

void *producer(void *ptr)
{
    Monitor *monitor = (Monitor *)(ptr);
    while (1)
        produce(monitor);
}



int consume(Monitor *this)
{
    pthread_mutex_lock(&this->mutex);

    while (this->readIndex == this->writeIndex && this->full == 0)
        pthread_cond_wait(&(this->cond), &(this->mutex));

    if(EXTENDED_PRINT) printf("Consumer get mutex \n");

    char *item = this->buffer[this->readIndex];

    if(strcmp("eof", item) == 0) return 0;

    consume_item(this->L,this->comparator,item);

    //free(this->buffer[this->writeIndex]);
    this->buffer[this->writeIndex] = NULL;

    this->readIndex = (this->readIndex + 1) % this->bufferSize;
    this->count--;

    //if (this->count == this->bufferSize - 1)
        if(this->full == 1) {
        this->full = 0;
        pthread_cond_broadcast(&(this->cond));
        }

    pthread_mutex_unlock(&this->mutex);

    return 1;
}

void *consumer(void *ptr)
{
    Monitor *monitor = (Monitor *)(ptr);
    while (consume(monitor))
        ;

    return 0;
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

    free(thr);

    return 0;
}