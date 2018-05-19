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

#include "common.h"

struct barberShop *BARBER_SHOP;
int SEMAPHORE_ID;
int SHARED_MEMORY_ID;


void go_to_sleep()
{
    BARBER_SHOP->barberStatus = SLEEP;
    print_text_and_pid("Barber is going sleep", getpid());
}

void wake_up()
{
    print_text_and_pid("Barber woke up", getpid());
    BARBER_SHOP->barberStatus = AWAKEN;
}

void invite_client()
{
    BARBER_SHOP->currentClient = pop(BARBER_SHOP);
    //change client status
    BARBER_SHOP->currentClient->clientStatus = INVITED;
    print_text_and_pid("Barber: invited client \n",  BARBER_SHOP->currentClient->clientPid);
    BARBER_SHOP->barberStatus = READY;
}

void invite_client_after_awake()
{
    printf("inivite client \n");
    //change client status
    BARBER_SHOP->currentClient->clientStatus = INVITED;
    print_text_and_pid("Barber: invited client \n",  BARBER_SHOP->currentClient->clientPid);
    BARBER_SHOP->barberStatus = READY;
}
void start_clipping()
{
    if(BARBER_SHOP->currentClient->clientStatus != ON_CHAIR) 
        return;
    print_text_and_pid("Barber is clipping \n", getpid());
    BARBER_SHOP->barberStatus = BUSY;
}

void end_clipping()
{
    print_text_and_pid("Barber finished clipping \n", getpid());

    //change client status
    BARBER_SHOP -> currentClient -> clientStatus = DONE;

    BARBER_SHOP->barberStatus = IDLE;
}

void clean_up() {
    if(SEMAPHORE_ID != 0) {
        semctl(SEMAPHORE_ID, 0, IPC_RMID);
    }
    if(SHARED_MEMORY_ID != 0) {
        shmctl(SHARED_MEMORY_ID, IPC_RMID, NULL);
    }
}

void handler_SIGTERM(int signo)
{
    printf("Barber received signal SIGTERM \n");
}

int create_shared_memory()
{
    key_t sharedMemoryKey = ftok(PROJECT_PATH, PROJECT_ID);

    int sharedMemoryID = shmget(sharedMemoryKey, sizeof(struct barberShop), 0666 | IPC_CREAT);

    SHARED_MEMORY_ID = sharedMemoryID;

    if (sharedMemoryID == -1)
        perror("Barber: failed to create shared memory\n");

    return sharedMemoryID;
}

void open_shared_memory(int sharedMemoryID)
{
    BARBER_SHOP = shmat(sharedMemoryID, 0, 0);

    if (BARBER_SHOP == (void *)-1)
        perror("Barber: faild to open shared memory\n");
}

void init_waiting_room(int numberOfSeats)
{
    if (numberOfSeats > MAX_WAITING_ROOM_SIZE)
        perror("Barber: wrong number of seats \n");

    int sharedMemoryID = create_shared_memory();

    open_shared_memory(sharedMemoryID);

    BARBER_SHOP->clientsInQueue = 0;
    BARBER_SHOP->currentClient = NULL;

    for (int i = 0; i < MAX_WAITING_ROOM_SIZE; i++)
        BARBER_SHOP->queue[i] = NULL;
}

void create_semaphore()
{
    key_t semaphoreKey = ftok(PROJECT_PATH, PROJECT_ID);

    SEMAPHORE_ID = semget(semaphoreKey, 1, IPC_CREAT | 0666);
}

void start_barbershop()
{
    semctl(SEMAPHORE_ID, 0, SETVAL, 0);
    BARBER_SHOP->barberStatus = IDLE;

    while (1)
    {
        get_lock(SEMAPHORE_ID);

        //printf("enter loop\n");

        switch (BARBER_SHOP->barberStatus)
        {
        case IDLE:
            if (BARBER_SHOP->clientsInQueue == 0)
            {
                go_to_sleep();
            }
            else
            {
                invite_client();
            }
            break;
        case CLIENT_WAKE_BARBER_UP:
            wake_up();
            break;
        case AWAKEN:
            invite_client_after_awake();
            break;
        case READY:
            start_clipping();
            break;
        case BUSY:
            end_clipping();
            break;
        default:
            //printf("no state were chosen\n");
            break;
        }

        realise_lock(SEMAPHORE_ID);
    }
}

int parse(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("Barber: wrong number of arguments\n");
    }

    return atoi(argv[1]);
}

int main(int argc, char *argv[])
{
    int numberOfSeats = parse(argc, argv);

    signal(SIGTERM,handler_SIGTERM);
    atexit(clean_up);

    init_waiting_room(numberOfSeats);

    create_semaphore();

    start_barbershop();

    return 0;
}
