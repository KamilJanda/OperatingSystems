#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

void add_return(char *line)
{
    line[strlen(line) - 1] = '\0';
}

char **split(char *str, const char *regex)
{
    char **res = NULL;
    char *p = strtok(str, regex);
    int n_spaces = 0;

    /* split string and append tokens to 'res' */
    while (p)
    {
        res = realloc(res, sizeof(char *) * ++n_spaces);

        if (res == NULL)
            exit(-1); /* memory allocation failed */

        res[n_spaces - 1] = p;

        p = strtok(0, regex);
    }

    /* realloc one extra element for the last NULL */
    res = realloc(res, sizeof(char *) * (n_spaces + 1));
    res[n_spaces] = 0;

    return res;
}

void run_task(char *arg[])
{
    if (execvp(arg[0], arg) < 0)
    {
        printf("ERROR: fail to run execvp\n");
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
        exit(EXIT_FAILURE);

    char **args = NULL;
    while (getline(&line, &length, fp) != -1)
    {
        add_return(line);
        args = split(line, " ");

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
            if (WEXITSTATUS(status))
            {
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