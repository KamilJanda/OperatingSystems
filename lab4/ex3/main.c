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

//only for OS x
#ifndef __linux__
#define SIGRTMIN 1
#define SIGRTMAX 8
#endif

int parentPid;
int PARENT_COUNT = 0;
int CHILD_COUNT = 0;
int SENT_SIGNALS = 0;

void parse(int argc, char *argv[]);
void create_child();
void handler_SIGUSR1_parent(int signum, siginfo_t *siginfo, void *context);
void handler_SIGUSR1_child(int signum, siginfo_t *siginfo, void *context);
void handler_SIGUSR2_async_parent(int signum, siginfo_t *siginfo, void *context);
void handler_SIGINT(int signum, siginfo_t *siginfo, void *context);

void set_handlers_parent();
void set_handlers_children();

void handler_SIGRTMAX_parent(int signum, siginfo_t *siginfo, void *context);
void handler_SIGRTMAX_child(int signum, siginfo_t *siginfo, void *context);

void send_signal(int type, pid_t pid);

// --- MAIN ---

int main(int argc, char *argv[])
{
    parse(argc, argv);
    parentPid = getpid();

    const int L = atoi(argv[1]);
    const int TYPE = atoi(argv[2]);

    create_child(L, TYPE);

    printf("Signal send: %d signals received from child: %d\n", SENT_SIGNALS, PARENT_COUNT);

    return 0;
}

void parse(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Wrong number of agruments \n");
        exit(EXIT_FAILURE);
    }
}

void create_child(int numberOfSignalToSend, int type)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // child code
        set_handlers_children();
        while (1)
        {
            pause();
        }
    }
    else if (pid > 0)
    {
        //parent code
        set_handlers_parent();

        sleep(3);

        for (int i = 0; i < numberOfSignalToSend; i++)
        {
            send_signal(type, pid);
        }

        if (type == 3)
        {
            kill(pid, SIGRTMIN);
        }
        else
        {
            kill(pid, SIGUSR2);
        }
    }
    else if (pid < 0)
    {
        printf("ERROR: pid < 0");
        exit(EXIT_FAILURE);
    }
}

void handler_SIGUSR1_parent(int signum, siginfo_t *siginfo, void *context)
{
    //printf("received SIGUSR1 from child\n");
    PARENT_COUNT++;
}
void handler_SIGUSR1_child(int signum, siginfo_t *siginfo, void *context)
{
    //printf("received SIGUSR1 from parent\n");
    CHILD_COUNT++;
    kill(getppid(), SIGUSR1);
}
void handler_SIGUSR2_child(int signum, siginfo_t *siginfo, void *context)
{
    printf("Received SIGUSR2 \n");
    printf("Signals received from parent: %d\n", CHILD_COUNT);
    exit(EXIT_SUCCESS);
}

void handler_SIGINT(int signum, siginfo_t *siginfo, void *context)
{
    printf("Received SIGINT \n");
}

void handler_SIGRTMAX_parent(int signum, siginfo_t *siginfo, void *context)
{
    //printf("received SIGUSR1 from child\n");
    PARENT_COUNT++;
}

void handler_SIGRTMAX_child(int signum, siginfo_t *siginfo, void *context)
{
    //printf("received SIGUSR1 from parent\n");
    CHILD_COUNT++;
    kill(getppid(), SIGRTMAX);
}

//handler for exit child
void handler_SIGRTMIN_child(int signum, siginfo_t *siginfo, void *context)
{
    printf("Received SIGRTMIN \n");
    printf("Signals received from parent: %d\n", CHILD_COUNT);
    exit(EXIT_SUCCESS);
}

void set_handlers_parent()
{
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);

    act.sa_sigaction = &handler_SIGUSR1_parent;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_sigaction = &handler_SIGRTMAX_parent;
    sigaction(SIGRTMAX, &act, NULL);
}

void set_handlers_children()
{
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);

    act.sa_sigaction = &handler_SIGUSR1_child;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_sigaction = &handler_SIGUSR2_child;
    sigaction(SIGUSR2, &act, NULL);

    act.sa_sigaction = &handler_SIGRTMAX_child;
    sigaction(SIGRTMAX, &act, NULL);

    act.sa_sigaction = &handler_SIGRTMIN_child;
    sigaction(SIGRTMIN, &act, NULL);
}

void send_signal(int type, pid_t pid)
{
    switch (type)
    {
    case 1:
        kill(pid, SIGUSR1);
        SENT_SIGNALS++;
        break;
    case 2:
        kill(pid, SIGUSR1);
        SENT_SIGNALS++;
        pause();
        break;
    case 3:
        kill(pid, SIGRTMAX);
        SENT_SIGNALS++;
        break;
    default:
        printf("Error incorrect type \n");
        exit(EXIT_FAILURE);
    }
}