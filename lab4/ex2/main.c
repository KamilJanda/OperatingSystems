#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>


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

int SHOW_ALL = 0;
int SHOW_CHILD_PID = 0;
int SHOW_REQUESTS = 0;
int SHOW_PERMISIONS = 0;
int SHOW_REAL_TIME = 0;
int SHOW_EXIT = 0;

pid_t parentPid;
pid_t *permisionRequests;
pid_t *children;

int requests = 0;
int bufferSize = 0;
int childrenAlive = 0;

void parse(int argc, char *argv[]);

void set_show_options(char *options);
void set_show_option(char option);

void child_action();

void create_children(int numberOfChilderen);

void handle_SIGUSR1(int signum, siginfo_t *siginfo, void *context);

void handle_SIGRT(int signum, siginfo_t *siginfo, void *context);

void handle_SIGCHLD(int signum, siginfo_t *siginfo, void *context);

void handle_SIGINT(int signum, siginfo_t *siginfo, void *context);

void kill_children();

void set_mask();

void set_handlers();



// --- MAIN ---

int main(int argc, char *argv[])
{
    parse(argc, argv);
    parentPid = getpid();

    set_handlers();

    create_children(N);

    while (childrenAlive > 0)
    {
    }

    printf("Number of children: %i\n", childrenAlive);

    return 0;
}

void parse(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Wrong number of agruments \n");
        printf("[number of children] [number of requests] [show options see blow]\n");
        printf("show options:\n");
        printf("all: -a\n");
        printf("child pid: -c\n");
        printf("received requests: -r\n");
        printf("sent permisions: -p\n");
        printf("received real time signals: -t\n");
        printf("exit of child: -e\n");
        exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    K = atoi(argv[2]);

    char *showOpt = argv[3];

    set_show_options(showOpt);

    permisionRequests = calloc(K, sizeof(pid_t));
    children = calloc(N, sizeof(pid_t));
}

void set_show_options(char *options)
{
    if (options[0] != '-')
    {
        printf("invalid last argument \n");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; options[i] != '\0'; i++)
    {
        set_show_option(options[i]);
    }
}

void set_show_option(char option)
{
    switch (option)
    {
    case 'c':
        SHOW_CHILD_PID = 1;
        break;
    case 'r':
        SHOW_REQUESTS = 1;
        break;
    case 'p':
        SHOW_PERMISIONS = 1;
        break;
    case 't':
        SHOW_REAL_TIME = 1;
        break;
    case 'e':
        SHOW_EXIT = 1;
        break;
    case 'a':
        SHOW_ALL = 1;
        break;
    default:
        break;
    }
}

void child_action()
{

    unsigned int sleepTime = (unsigned int)rand() % 11;
    sleep(sleepTime);

    if (SHOW_ALL)
        printf("Send signal SIGUSR1 from: %d after sleep for: %d\n", getpid(), sleepTime);

    kill(getppid(), SIGUSR1);

    pause();

    int realTimeSignal = SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN));

    kill(getppid(), realTimeSignal);

    exit(sleepTime);
}

void create_children(int numberOfChilderen)
{
    for (int i = 0; i < numberOfChilderen; i++)
    {

        pid_t pid = fork();
        childrenAlive++;

        if (pid == 0)
        {
            // child code
            child_action();
        }
        else if (pid > 0)
        {
            children[i] = pid;

            if (SHOW_CHILD_PID || SHOW_ALL)
                printf("Created child with PID %i\n", pid);
        }
        else if (pid < 0)
        {
            printf("ERROR: pid < 0");
            exit(EXIT_FAILURE);
        }
    }
}

void handle_SIGUSR1(int signum, siginfo_t *siginfo, void *context)
{
    if (getpid() != parentPid)
        return;

    requests++;

    if (SHOW_REQUESTS || SHOW_ALL)
        printf("Received request (signal: %i) from: %i \n", signum, siginfo->si_pid);

    if (requests < K)
    {
        permisionRequests[bufferSize] = siginfo->si_pid;
        bufferSize++;
    }
    else if (requests == K)
    {

        if (SHOW_ALL)
            printf("---------------- requests == K ----------------\n");

        permisionRequests[bufferSize] = siginfo->si_pid;
        bufferSize++;

        for (int i = 0; i < bufferSize; i++)
        {
            kill(permisionRequests[i], SIGUSR1);

            if (SHOW_PERMISIONS || SHOW_ALL)
                printf("Send permision to use real time sig to pid: %d \n", permisionRequests[i]);
        }
    }
    else
    {
        kill(siginfo->si_pid, SIGUSR1);
    }
}

void handle_SIGRT(int signum, siginfo_t *siginfo, void *context)
{
    if (getpid() != parentPid)
        return;

    if (SHOW_REAL_TIME || SHOW_ALL)
        printf("receive real time signal: %d from: %d \n", signum, siginfo->si_pid);
}

void handle_SIGCHLD(int signum, siginfo_t *siginfo, void *context)
{

    if (parentPid != getpid())
        return;

    for (int i = 0; i < N; i++)
    {
        if (children[i] == siginfo->si_pid)
            children[i] = 0;
    }

    childrenAlive--;

    if (SHOW_EXIT || SHOW_ALL)
        printf("EXIT child pid: %d with status: %d\n", siginfo->si_pid, siginfo->si_status);
}

void handle_SIGINT(int signum, siginfo_t *siginfo, void *context)
{
    if (getpid() != parentPid)
        return;

    kill_children();

    exit(EXIT_SUCCESS);
}

void kill_children()
{
    for (int i = 0; i < N; i++)
    {
        if (children[i] != 0)
        {

            if (SHOW_EXIT || SHOW_ALL)
                printf("Terminate child pid: %d \n", children[i]);

            kill(children[i], SIGKILL);
            childrenAlive--;
        }
    }
}

void set_handlers()
{
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);

    act.sa_sigaction = &handle_SIGUSR1;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_sigaction = &handle_SIGCHLD;
    sigaction(SIGCHLD, &act, NULL);

    act.sa_sigaction = &handle_SIGINT;
    sigaction(SIGINT, &act, NULL);

    act.sa_sigaction = &handle_SIGRT;
    for (int i = SIGRTMIN; i <= SIGRTMAX; i++)
    {
        sigaction(i, &act, NULL);
    }
}