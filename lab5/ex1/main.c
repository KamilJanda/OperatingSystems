#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define WRITE_END 1
#define READ_END 0

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void add_return(char *line)
{
    if (line[strlen(line) - 1] == '\n')
    {
        line[strlen(line) - 1] = '\0';
    }
    else
    {
        line = concat(line, "\0");
    }
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

        if (numberOfCommands > 10)
        {
            printf("error: Max number of command is 10\n");
            exit(EXIT_FAILURE);
        }

        int **pipefd = calloc(numberOfCommands, sizeof(int *));
        for (int i = 0; i < numberOfCommands; i++)
        {
            pipefd[i] = calloc(2, sizeof(int));
        }

        for (int i = 0; i < numberOfCommands; i++)
        {
            if (pipe(pipefd[i]) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < numberOfCommands; i++)
        {

            add_return(commands[i]);
            args = split(commands[i], " ");

            pid_t pid = fork();

            if (pid == 0)
            {
                if (i != numberOfCommands - 1)
                {

                    int status = dup2(pipefd[i][WRITE_END], STDOUT_FILENO);
                    if (status < 0)
                    {
                        printf("error of dup2\n");
                        exit(EXIT_FAILURE);
                    }
                }
                if (i != 0)
                {

                    int status = dup2(pipefd[i - 1][READ_END], STDIN_FILENO);
                    if (status < 0)
                    {
                        printf("error of dup2\n");
                        exit(EXIT_FAILURE);
                    }
                }

                for (int i = 0; i < numberOfCommands; i++)
                {
                    close(pipefd[i][READ_END]);
                    close(pipefd[i][WRITE_END]);
                }

                run_task(args);
                exit(EXIT_SUCCESS);
            }
            else if (pid < 0)
            {
                printf("ERROR: pid < 0");
                exit(EXIT_FAILURE);
            }

            free(args);
        }

        for (int i = 0; i < numberOfCommands; i++)
        {
            close(pipefd[i][READ_END]);
            close(pipefd[i][WRITE_END]);
        }

        pid_t pid;
        int status = 0;
        while ((pid = waitpid(-1, &status, 0)) > 0)
        {
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            {
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(fp);
    if (line)
        free(line);

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