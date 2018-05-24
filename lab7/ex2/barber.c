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

#include "common.h"

struct barberShop *BARBER_SHOP;
int SEMAPHORE;
int SHARED_MEMORY_FD;

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

    BARBER_SHOP->currentClientPid = pop(BARBER_SHOP);

    print_text_and_pid("Barber: invited client ", BARBER_SHOP->currentClientPid);
    BARBER_SHOP->barberStatus = READY;
}

void invite_client_after_awake()
{
    printf("inivite client %d \n", BARBER_SHOP->currentClientPid);

    print_text_and_pid("Barber: invited client after awake", BARBER_SHOP->currentClientPid);
    BARBER_SHOP->barberStatus = READY;
}

void end_clipping()
{
    print_text_and_pid("Barber finished clipping \n", BARBER_SHOP->currentClientPid);

    BARBER_SHOP->barberStatus = FINISHED;
}

void start_clipping()
{

    print_text_and_pid("Barber is clipping \n", BARBER_SHOP->currentClientPid);

    BARBER_SHOP->barberStatus = BUSY;

    end_clipping();
}

void clean_up()
{
    if (SEMAPHORE != 0)
        sem_unlink(PROJECT_PATH);
    if (SHARED_MEMORY_FD != 0)
        shm_unlink(PROJECT_PATH);
}

void handler_SIGTERM(int signo)
{
    printf("Barber received signal SIGTERM \n");
}

void create_shared_memory()
{
    SHARED_MEMORY_FD = shm_open(
        PROJECT_PATH,
        O_RDWR | O_CREAT | O_EXCL,
        S_IRWXU | S_IRWXG);

    ftruncate(SHARED_MEMORY_FD, sizeof(*BARBER_SHOP));
}

void open_shared_memory()
{
    BARBER_SHOP = mmap(NULL, sizeof(*BARBER_SHOP), PROT_READ | PROT_WRITE, MAP_SHARED, SHARED_MEMORY_FD, 0);

    if (BARBER_SHOP == (void *)-1)
        perror("Barber: faild to open shared memory\n");
}

void init_waiting_room(int numberOfSeats)
{
    if (numberOfSeats > MAX_WAITING_ROOM_SIZE)
        perror("Barber: wrong number of seats \n");

    create_shared_memory();

    open_shared_memory();

    BARBER_SHOP->clientsInQueue = 0;
    BARBER_SHOP->currentClient = NULL;

    for (int i = 0; i < MAX_WAITING_ROOM_SIZE; i++)
        BARBER_SHOP->queue[i] = 0;
}

void create_semaphore()
{
    SEMAPHORE = sem_open(PROJECT_PATH, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, 0);
}

void start_barbershop()
{
    BARBER_SHOP->barberStatus = IDLE;

    while (1)
    {
        get_lock(SEMAPHORE);

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
        default:
            break;
        }

        realise_lock(SEMAPHORE);
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

    signal(SIGTERM, handler_SIGTERM);
    atexit(clean_up);

    init_waiting_room(numberOfSeats);

    create_semaphore();

    start_barbershop();

    return 0;
}
