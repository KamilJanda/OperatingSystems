#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <zconf.h>
#include <semaphore.h>

#define MAX_WAITING_ROOM_SIZE 20

#define PROJECT_PATH "/barbershop"


//Barber status
#define SLEEP 1
#define AWAKEN 2
#define READY 3
#define IDLE 4
#define BUSY 5
#define CLIENT_WAKE_BARBER_UP 6
#define FINISHED 7

//Client status
#define NEW_CLIENT 1
#define INVITED 2
#define WAIT 3
#define LEFT 4
#define ON_CHAIR 5
#define DONE 6
#define WOKE_BARBER_UP 7

/* struct barberShop *BARBER_SHOP;
int SEMAPHORE_ID;
int SHARED_MEMORY_ID; */

struct client
{
    pid_t clientPid;
    int clientStatus;
};

struct barberShop
{
    int barberStatus;
    int clientsInQueue;
    pid_t queue[MAX_WAITING_ROOM_SIZE];
    struct client *currentClient;
    pid_t currentClientPid;
};

//QUEUE/////

void add(pid_t cleint, struct barberShop *barberShop)
{
    if (barberShop->clientsInQueue == MAX_WAITING_ROOM_SIZE)
        return;

    barberShop->queue[barberShop->clientsInQueue] = cleint;
    barberShop->clientsInQueue++;
}

pid_t pop(struct barberShop *barberShop)
{
    if (barberShop->clientsInQueue == 0)
        return -1;

    pid_t result = barberShop->queue[0];

    for (int i = 0; i < barberShop->clientsInQueue - 1; i++)
    {
        barberShop->queue[i] = barberShop->queue[i + 1];
    }

    barberShop->queue[barberShop->clientsInQueue - 1] = 0;
    barberShop->clientsInQueue--;

    return result;
}

int is_empty(struct barberShop *barberShop)
{
    if (barberShop->clientsInQueue == 0)
        return 1;
    return 0;
}

int is_full(struct barberShop *barberShop)
{
    if (barberShop->clientsInQueue >= MAX_WAITING_ROOM_SIZE)
        return 1;
    return 0;
}

//Semaphore

void realise_lock(sem_t* semaphore)
{
    int error = sem_post(semaphore);
    if (error == -1) perror("Semaphore error: %s");
}

void get_lock(sem_t* semaphore)
{
    int error = sem_wait(semaphore);
    if (error == -1) perror("Semaphore error: %s");
}

//print

void print_text_and_pid(char *text, pid_t pid)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    printf("pid: %d %s [ %lld.%.9ld ] \n", pid, text, (long long)tp.tv_sec, tp.tv_nsec);
    fflush(stdout);

    //printf("%s pid: %d \n",text,pid);
};

#endif