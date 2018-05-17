#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <sys/types.h>

#define PROJECT_ID 1
#define MAX_TEXT_SIZE 1024
#define MAX_CLIENTS 10


#define MIRROR 1
#define CALC 2
#define TIME 3
#define END 4
#define REGISTER 5
#define QUIT 6

struct msgBuf{
    long mtype;
    pid_t pid;
    char text[MAX_TEXT_SIZE];
};

struct clientsInfo{
    int used;
    pid_t pid;
    int id;
    int clientQueueDesc;
};

const char serverPath[] = "/server";

#endif
