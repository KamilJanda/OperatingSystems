#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define errExit(msg)        \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

void add_return(char *line)
{
    line[strlen(line) - 1] = '\0';
}

char **split(char *str, const char *regex)
{
    char **res = NULL;
    char *record = strtok(str, regex);
    int n_spaces = 0;

    while (record)
    {
        res = realloc(res, sizeof(char *) * ++n_spaces);

        if (res == NULL)
        {
            printf("memory allocation failed \n");
            exit(EXIT_FAILURE);
        }

        res[n_spaces - 1] = record;

        record = strtok(0, regex);
    }

    res = realloc(res, sizeof(char *) * (n_spaces + 1));
    res[n_spaces] = 0;

    return res;
}

void run_task(char *arg[])
{
    if (execvp(arg[0], arg) < 0)
    {
        printf("Error execvp: fail to run %s\n", arg[0]);
        exit(EXIT_FAILURE);
    }
}

void set_limits(int time, int memory)
{
    struct rlimit timeLimit;
    struct rlimit memoryLimit;

    timeLimit.rlim_cur = (rlim_t)time;
    timeLimit.rlim_max = (rlim_t)time;

    memoryLimit.rlim_cur = (rlim_t)(memory * 1024 * 1024);
    memoryLimit.rlim_max = (rlim_t)(memory * 1024 * 1024);

    if (setrlimit(RLIMIT_CPU, &timeLimit) == -1)
        errExit("Fail to set time limit");

    if (setrlimit(RLIMIT_AS, &memoryLimit) == -1)
        errExit("Fail to set memory limit");
}

void print_usage(char *name, struct rusage after, struct rusage before)
{
    time_t userTime =
        ((after.ru_utime.tv_sec * 1000000) + after.ru_utime.tv_usec) -
        ((before.ru_utime.tv_sec * 1000000) + before.ru_utime.tv_usec);

    time_t systemTime =
        ((after.ru_stime.tv_sec * 1000000) + after.ru_stime.tv_usec) -
        ((before.ru_stime.tv_sec * 1000000) + before.ru_stime.tv_usec);

    printf("Execution of '%s' user time: %ld us system time: %ld us \n\n",
           name, userTime, systemTime);
}

void process_file_with_limits(char *fileName, int time, int memory)
{
    FILE *fp;
    char *line = NULL;
    size_t length = 0;

    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Error: cannot open file \n");
        exit(EXIT_FAILURE);
    }

    char **args = NULL;

    while (getline(&line, &length, fp) != -1)
    {
        struct rusage before; //new for ex3
        struct rusage after;

        add_return(line);
        args = split(line, " ");

        getrusage(RUSAGE_CHILDREN, &before); //new for ex3

        pid_t pid = fork();

        if (pid == 0)
        {

            set_limits(time, memory); //new for ex3

            run_task(args);
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0)
        {
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            {
                printf("Failed of: %s", args[0]);
                exit(EXIT_FAILURE);
            }
        }
        else if (pid < 0)
        {
            printf("ERROR: pid < 0");
            exit(EXIT_FAILURE);
        }

        getrusage(RUSAGE_CHILDREN, &after);  //new for ex3
        print_usage(args[0], after, before); // new for ex3

        free(args);
    }

    fclose(fp);
    if (line)
        free(line);

    exit(EXIT_SUCCESS);
}

int parse(int argc, char *argv[])
{
    switch (argc)
    {
    case 1:
    case 2:
    case 3:
        printf("Too few arguments\n");
        exit(EXIT_FAILURE);
    case 4:
        return 1;
    default:
        printf("Too many arguments\n");
        exit(EXIT_FAILURE);
    }
}

// --- MAIN ---

int main(int argc, char *argv[])
{
    parse(argc, argv);

    int time = atoi(argv[2]);
    int memory = atoi(argv[3]);

    process_file_with_limits(argv[1], time, memory);

    return 0;
}