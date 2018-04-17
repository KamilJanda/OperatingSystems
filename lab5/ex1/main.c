#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/wait.h>

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

void process_file(char *fileName)
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
        char **commands = split(line, "|");
        int numberOfCommands = 0;
        while (commands[numberOfCommands] != 0)
            numberOfCommands++;

        for (int i = 0; i < numberOfCommands; i++)
        {

            add_return(commands[i]);
            args = split(commands[i], " ");

            pid_t pid = fork();

            if (pid == 0)
            {
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

            free(args);
        }
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
        printf("Too few arguments\n");
        exit(EXIT_FAILURE);
    case 2:
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

    process_file(argv[1]);

    return 0;
}