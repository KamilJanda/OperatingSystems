#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

//to remove
#ifndef __linux__
#define SIGRTMIN 1
#define SIGRTMAX 8
#endif

#define SIGINT 2
#ifdef __linux__
#define SIGSTP 20
#else
#define SIGSTP 18 // According to `man 3 signal` on MacOS
#endif

int N, K;

pid_t parentPid;
pid_t *pids;
int requests = 0;
int bufferSize = 0;

void parse(int argc, char *argv[]);
void child_action();
void create_children(int numberOfChilderen);
void handle_SIGUSR1(int signum, siginfo_t *siginfo, void * context);
void set_mask();

// --- MAIN ---

int main(int argc, char *argv[])
{
    parse(argc, argv);

    parentPid = getpid();

    return 0;
}

void parse(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Wrong number of agruments \n");
        exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    K = atoi(argv[2]);
}

void child_action()
{
    int sleepTime = rand() % 11;
    sleep(sleepTime);

    kill(getppid(), SIGUSR1);

    pause();

    int realTimeSignal = SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN));
    union sigval sigValue;
    sigValue.sival_int = sleepTime;

    sigqueue(getppid(), realTimeSignal, sigValue);
    

    exit(sleepTime);
}

void create_children(int numberOfChilderen)
{
    for (int i = 0; i < numberOfChilderen; i++)
    {

        pid_t pid = fork();
        pid_t childPid = -1;

        if (pid == 0)
        {
            // child code
            child_action();
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
}

void handle_SIGUSR1(int signum, siginfo_t *siginfo, void * context)
{
    if (getpid() != parentPid)
        return;

    requests++;

    if (requests < K)
    {
        pids[bufferSize] = siginfo->si_pid;
        bufferSize++;
    }
    else if (requests == K)
    {
        pids[bufferSize] = siginfo->si_pid;
        bufferSize++;

        for (int i = 0; i < bufferSize; i++)
        {
            kill(pids[i], SIGUSR1);
            printf("send permision to use real time sig to pid: %d \n",pids[i]);
        }
    }
    else
    {
        kill(siginfo->si_pid, SIGUSR1);
    }
}

void set_mask()
{
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
}
