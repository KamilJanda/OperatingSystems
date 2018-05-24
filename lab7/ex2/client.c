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

#include <errno.h>

#include "common.h"

struct barberShop *BARBER_SHOP;
sem_t* semaphore;
int SHARED_MEMORY_FD;

struct client *CURRENT_CLIENT;

void wake_barber_up()
{
    print_text_and_pid("Client woke barber up", getpid());

    //change barber status
    BARBER_SHOP->barberStatus = CLIENT_WAKE_BARBER_UP;

    CURRENT_CLIENT->clientStatus = WOKE_BARBER_UP;

    BARBER_SHOP->currentClientPid = getpid();

    printf("changerd barber stat to awake \n");
}

void take_chair()
{
    print_text_and_pid("Client took a chair", getpid());

    BARBER_SHOP->barberStatus = READY;

    CURRENT_CLIENT->clientStatus = DONE;
}

void leave_after_clipping()
{
    if (BARBER_SHOP->barberStatus != FINISHED)
    {
        return;
    }

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

        while (left != 1)
        {
            get_lock(semaphore);

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

            realise_lock(semaphore);
        }

        free(CURRENT_CLIENT);
    }
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

void create_shared_memory()
{
    SHARED_MEMORY_FD = shm_open(PROJECT_PATH, O_RDWR, S_IRWXU | S_IRWXG);

    ftruncate(SHARED_MEMORY_FD, sizeof(*BARBER_SHOP));
}

void open_shared_memory()
{
    BARBER_SHOP = mmap(NULL,
                      sizeof(*BARBER_SHOP),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      SHARED_MEMORY_FD,
                      0);

    if (BARBER_SHOP == (void *)-1)
        perror("Client: faild to open shared memory\n");
}

void set_up_memory()
{
    create_shared_memory();
    open_shared_memory();
}

void clean_up()
{
    if (semaphore != 0) sem_unlink(PROJECT_PATH);
    if (SHARED_MEMORY_FD != 0) shm_unlink(PROJECT_PATH);
}

void create_semaphore()
{
    semaphore = sem_open(PROJECT_PATH, O_WRONLY, S_IRWXU | S_IRWXG, 0);

    if (semaphore == (void*) -1)
        perror("Couldn't create semaphore\n");
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

    //atexit(clean_up);

    set_up_memory();
    create_semaphore();

    pid_t pid;

    for (int i = 0; i < numberOfClients; i++)
    {
        
        pid = fork();
        if (pid == 0)
        {
            run_client(numberOfClipping);
            exit(EXIT_SUCCESS);
        }
    }

    

    return 0;
}