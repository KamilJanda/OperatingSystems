#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIGINT 2
#ifdef __linux__
#define SIGSTP 20
#else
#define SIGSTP 18 // for OSx
#endif

int isPaused = 0;

void handler_SIGTSTP(int signum);
void handler_SIGINT(int signum);
void print_time();

// --- MAIN ---

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = handler_SIGTSTP;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGTSTP, &act, NULL);

    signal(SIGINT, handler_SIGINT);

    while (1)
    {
        print_time();
        sleep(1);
        if (isPaused)
            pause();
    }

    return 0;
}

void handler_SIGTSTP(int signum)
{
    if (isPaused)
    {
        isPaused = 0;
    }
    else
    {
        isPaused = 1;
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    }
}

void handler_SIGINT(int signum)
{
    printf("\nOdebrano sygnał SIGINT\n");
    exit(EXIT_SUCCESS);
}

void print_time()
{
    time_t raw_time;
    struct tm *time_info;

    time(&raw_time);
    time_info = localtime(&raw_time);
    printf("%s", asctime(time_info));
}