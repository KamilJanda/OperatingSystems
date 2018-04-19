#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int parse(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few agruments\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 2)
    {
        printf("Too many agruments\n");
        exit(EXIT_FAILURE);
    }

    return 1;
}

int main(int argc, char *argv[])
{
    parse(argc, argv);

    char *path = argv[1];

    if (mkfifo(path, S_IRUSR | S_IWUSR) == -1)
    {
        perror("Master: failed to create FIFO\n");
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    FILE *fifo = fopen(path, "r");
    if (fifo == NULL)
    {
        perror("Master: Fail to open FIFO\n");
        exit(EXIT_FAILURE);
    }
    while ((nread = getline(&line, &len, fifo)) != -1)
    {
        printf("%s\n", line);
    }

    return 0;
}