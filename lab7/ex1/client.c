#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <time.h>

#include <errno.h>

#include "common.h"

struct barberShop *BARBER_SHOP;
int SEMAPHORE_ID;
int SHARED_MEMORY_ID;

struct client *CURRENT_CLIENT;

void wake_barber_up()
{
    print_text_and_pid("Client woke barber up", getpid());

    //change barber status
    BARBER_SHOP->barberStatus = CLIENT_WAKE_BARBER_UP;

    CURRENT_CLIENT->clientStatus = WOKE_BARBER_UP;
    //BARBER_SHOP -> currentClient = CURRENT_CLIENT;

    BARBER_SHOP->currentClientPid = getpid();

    printf("changerd barber stat to awake \n");
}

void take_chair()
{
    print_text_and_pid("Client took a chair", getpid());

    BARBER_SHOP->barberStatus = READY;
    //CURRENT_CLIENT -> clientStatus = ON_CHAIR;
    CURRENT_CLIENT->clientStatus = DONE;
}

void leave_after_clipping()
{
    if (BARBER_SHOP->barberStatus != FINISHED)
    {
        return;
    }

    /* while (1)
    {
        realise_lock(SEMAPHORE_ID);
        get_lock(SEMAPHORE_ID);
        if (BARBER_SHOP->barberStatus == FINISHED)
            break;
    } */

    print_text_and_pid("Leave barber after clipping", getpid());
    BARBER_SHOP->barberStatus = IDLE;

    CURRENT_CLIENT->clientStatus = LEFT;
    //BARBER_SHOP->currentClient = -1;
}

void sit_in_waiting_room()
{
    print_text_and_pid("Client sit in waiting room", getpid());
    CURRENT_CLIENT->clientStatus = WAIT;
    add(getpid(), BARBER_SHOP);
}

void no_sits_leave()
{
    print_text_and_pid("No sits client left", getpid());
    CURRENT_CLIENT->clientPid = LEFT;
}

void check_for_sit()
{
    if (is_full(BARBER_SHOP))
    {
        no_sits_leave();
    }
    else
    {
        sit_in_waiting_room();
    }
}

void check_barber()
{
    if (BARBER_SHOP->barberStatus == SLEEP)
    {
        wake_barber_up();
    }
    else
    {
        check_for_sit();
    }
}

void wait_for_barber()
{
    if (BARBER_SHOP->barberStatus == READY && BARBER_SHOP->currentClientPid == getpid())
    {
        CURRENT_CLIENT->clientStatus = INVITED;
        printf("Changed client stat to ivited \n");
    }
}

void run_client(int numberOfClipping)
{
    for (int i = 0; i < numberOfClipping; i++)
    {
        CURRENT_CLIENT = malloc(sizeof(struct client));

        CURRENT_CLIENT->clientStatus = NEW_CLIENT;
        CURRENT_CLIENT->clientPid = getpid();

        int left = 0;

        //printf("tataj> %d\n",getpid());

        while (left != 1)
        {
            //printf("before lock \n");

            get_lock(SEMAPHORE_ID);

            //printf("get lock \n");

            switch (CURRENT_CLIENT->clientStatus)
            {
            case NEW_CLIENT:
                check_barber();
                break;
            case LEFT:
                left = 1;
                break;
            case INVITED:
                take_chair();
                break;
            case DONE:
                leave_after_clipping();
                break;
            case WOKE_BARBER_UP:
                wait_for_barber();
                break;
            case WAIT:
                wait_for_barber();
                break;
            default:
                break;
            }

            //printf("-----Barber: %d Client: %d queue: %d\n" ,BARBER_SHOP->barberStatus,CURRENT_CLIENT->clientStatus,BARBER_SHOP->clientsInQueue);

            realise_lock(SEMAPHORE_ID);
        }

        free(CURRENT_CLIENT);
    }

    //exit(EXIT_SUCCESS);
}

void spaw_client(int numberOfClipping)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        run_client(numberOfClipping);
    }
}

//Helpers

int create_shared_memory()
{
    key_t sharedMemoryKey = ftok(PROJECT_PATH, PROJECT_ID);

    //int sharedMemoryID = shmget(sharedMemoryKey, sizeof(struct barberShop), 0);
    int sharedMemoryID = shmget(sharedMemoryKey, 0, 0);

    SHARED_MEMORY_ID = sharedMemoryID;

    if (sharedMemoryID == -1)
        perror("Client: failed to create shared memory\n");

    return sharedMemoryID;
}

void open_shared_memory(int sharedMemoryID)
{
    BARBER_SHOP = shmat(sharedMemoryID, 0, 0);

    if (BARBER_SHOP == (void *)-1)
        perror("Client: faild to open shared memory\n");
}

void set_up_memory()
{
    SHARED_MEMORY_ID = create_shared_memory();
    open_shared_memory(SHARED_MEMORY_ID);
}

void clean_up()
{
    if (SEMAPHORE_ID != 0)
    {
        semctl(SEMAPHORE_ID, 0, IPC_RMID);
    }
    if (SHARED_MEMORY_ID != 0)
    {
        shmctl(SHARED_MEMORY_ID, IPC_RMID, NULL);
    }
}

void create_semaphore()
{
    key_t semaphoreKey = ftok(PROJECT_PATH, PROJECT_ID);

    SEMAPHORE_ID = semget(semaphoreKey, 1, IPC_CREAT | 0666);
    //SEMAPHORE_ID = semget(semaphoreKey, 0, 0);

    if(SEMAPHORE_ID == -1)
        perror("Client failed to ceate semaphore \n");
}

void parse(int argc, char *argv[], int *numberOfClients, int *numberOfClipping)
{
    if (argc != 3)
        perror("Client: wrong numer of arguments\n");

    *numberOfClients = atoi(argv[1]);
    *numberOfClipping = atoi(argv[2]);
}

int main(int argc, char *argv[])
{
    int numberOfClients;
    int numberOfClipping;

    parse(argc, argv, &numberOfClients, &numberOfClipping);

    atexit(clean_up);

    set_up_memory();
    create_semaphore();

    pid_t pid;

    for (int i = 0; i < numberOfClients; i++)
    {
        //spaw_client(numberOfClipping);
        pid = fork();
        if (pid == 0)
        {
            run_client(numberOfClipping);
            exit(EXIT_SUCCESS);
        }
    }

    while (wait(0))
        if (errno != ECHILD)
            break;

    
        /* pid_t wpid;
        int status = 0;
        while ((wpid = wait(&status)) > 0)
            ; */
    

    //printf("-----------------------main tutaj \n");

    

    return 0;
}