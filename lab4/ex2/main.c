#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>



#define SIGINT 2
#ifdef __linux__
#define SIGSTP 20
#else
#define SIGSTP 18 // According to `man 3 signal` on MacOS
#endif

#define ALL 1
#define SHORT 2

const int SHOW_TYPE = ALL; 

int N, K;

pid_t parentPid;
pid_t *permisionRequests;
pid_t *children;

int requests = 0;
int bufferSize = 0;
int childrenAlive = 0;

void parse(int argc, char *argv[]);

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

int main(int argc, char *argv[]) {
    parse(argc, argv);
    parentPid = getpid();

    set_handlers();

    create_children(N);

    while (childrenAlive > 0){ }

    #if SHOW_TYPE == ALL
    printf("Number of children: %i\n",childrenAlive);
    #endif

    return 0;
}

void parse(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong number of agruments \n");
        exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    K = atoi(argv[2]);

    permisionRequests = calloc(K, sizeof(pid_t));
    children = calloc(N, sizeof(pid_t));
}

void child_action() {

    unsigned int sleepTime =(unsigned int) rand() % 11;
    sleep(sleepTime);

    #if SHOW_TYPE == ALL || SHOW_TYPE == SHORT
    printf("Send signal SIGUSR1 from: %d after sleep for: %d\n",getpid(),sleepTime);
    #endif
    kill(getppid(), SIGUSR1);

    pause();

    int realTimeSignal = SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN));
    
    kill(getppid(), realTimeSignal);

    exit(sleepTime);
}

void create_children(int numberOfChilderen) {
    for (int i = 0; i < numberOfChilderen; i++) {

        pid_t pid = fork();
        childrenAlive++;

        if (pid == 0) {
            // child code
            child_action();
        } else if (pid > 0) {
            children[i] = pid;
            #if SHOW_TYPE == ALL
            printf("Created child with PID %i\n", pid);
            #endif
        } else if (pid < 0) {
            printf("ERROR: pid < 0");
            exit(EXIT_FAILURE);
        }
    }
}

void handle_SIGUSR1(int signum, siginfo_t *siginfo, void *context) {
    if (getpid() != parentPid)
        return;

    requests++;
    #if SHOW_TYPE == ALL || SHOW_TYPE == SHORT
    printf("Received request (signal: %i) from: %i \n", signum ,siginfo->si_pid);
    #endif

    if (requests < K) {
        permisionRequests[bufferSize] = siginfo->si_pid;
        bufferSize++;
    } else if (requests == K) {
        #if SHOW_TYPE == ALL
        printf("---------------- requests == K ----------------\n");
        #endif

        permisionRequests[bufferSize] = siginfo->si_pid;
        bufferSize++;

        for (int i = 0; i < bufferSize; i++) {
            kill(permisionRequests[i], SIGUSR1);
            #if SHOW_TYPE == ALL || SHOW_TYPE == SHOR
            printf("Send permision to use real time sig to pid: %d \n", permisionRequests[i]);
            #endif
        }
    } else {
        kill(siginfo->si_pid, SIGUSR1);
    }
}

void handle_SIGRT(int signum, siginfo_t *siginfo, void *context) {
    if (getpid() != parentPid)
        return;

    #if SHOW_TYPE == ALL || SHOW_TYPE == SHOR
    printf("receive real time signal: %d from: %d \n", signum, siginfo->si_pid);
    #endif

}

void handle_SIGCHLD(int signum, siginfo_t *siginfo, void *context) {

    if (parentPid != getpid()) return;

    for(int i=0;i<N;i++)
    {
        if(children[i] == siginfo->si_pid) children[i] = 0;
    }

    childrenAlive--;

    #if SHOW_TYPE == ALL || SHOW_TYPE == SHOR
    printf("EXIT child pid: %d with status: %d\n",siginfo->si_pid,siginfo->si_status);
    #endif
}

void handle_SIGINT(int signum, siginfo_t *siginfo, void *context)
{
    if(getpid() != parentPid)
        return;

    kill_children();

    exit(EXIT_SUCCESS);
}

void kill_children()
{
    for(int i=0;i<N;i++)
    {
        if(children[i] != 0)
        {
            #if SHOW_TYPE == ALL
            printf("Terminate child pid: %d \n",children[i]);
            #endif
            kill(children[i],SIGINT);
            childrenAlive--;
        }
    } 
}

void set_handlers() {
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
    for (int i = SIGRTMIN; i <= SIGRTMAX; i++) {
        sigaction(i, &act, NULL);
    }
}