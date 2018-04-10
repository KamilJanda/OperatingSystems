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
#define SIGSTP 18 // According to `man 3 signal` on MacOS
#endif

#define PATH_DATE "./data.sh"

int isChildAlive = 0;
pid_t childPid = 0;

void handler_SIGTSTP(int signum);
void handler_SIGINT(int signum);
void print_time();
void create_child();

// --- MAIN ---

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = handler_SIGTSTP;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGTSTP, &act, NULL);

    signal(SIGTSTP, handler_SIGTSTP);

    create_child();

    while(1);

    return 0;
}

void handler_SIGTSTP(int signum)
{
    if (isChildAlive)
    {
        isChildAlive = 0;
        kill(childPid,SIGKILL);
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    }
    else
    {
        create_child();
    }
    signal(SIGTSTP, handler_SIGTSTP);
}

void handler_SIGINT(int signum)
{
    printf("\nOdebrano sygnał SIGINT\n");
    if(isChildAlive) kill(childPid,SIGKILL);
    signal(SIGINT, SIG_DFL);
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

void create_child()
{
    pid_t pid = fork();
    isChildAlive = 1;

    if (pid == 0)
    {
        char* args[2] = {PATH_DATE, NULL};
        execvp(PATH_DATE, args);
        exit(EXIT_SUCCESS);
    }
    else if (pid > 0)
    {
        childPid = pid;
    }
    else if (pid < 0)
    {
        printf("ERROR: pid < 0");
        exit(EXIT_FAILURE);
    }
}