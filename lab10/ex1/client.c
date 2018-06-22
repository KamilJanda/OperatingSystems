#include "common.h"

char *NAME;
char *SERVER_ADDRESS;
char *SOCKET_PATH;
unsigned short PORT;
char *MODE;
int SOCKETD;

void handle_signal(int signum)
{
    printf("Received SIGINT closing\n");
    exit(EXIT_SUCCESS);
}

void clean_up(void)
{
    shutdown(SOCKETD, SHUT_RDWR);
    close(SOCKETD);
}

int init_local_socket()
{
    int local = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    connect(local, (struct sockaddr *)&(addr), sizeof(addr));
    return local;
}

int init_inet_socket() {
    int inet = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_ADDRESS, &addr.sin_addr);

    connect(inet, (struct sockaddr*) &(addr), sizeof(addr));
    return inet;
}

void login_client()
{
    msg message;
    strcpy(message.name, NAME);
    message.type = LOG;

    write(SOCKETD, &message, sizeof(message));
}

void reply(expression e, int id)
{

    int result;
    switch (e.type)
    {
    case DIV:
    {
        result = e.arg1 / e.arg2;
        break;
    }
    case SUB:
    {
        result = e.arg1 - e.arg2;
        break;
    }
    case SUM:
    {
        result = e.arg1 + e.arg2;
        break;
    }
    case MUL:
    {
        result = e.arg1 * e.arg2;
        break;
    }
    }

    msg message;
    strcpy(message.name, NAME);
    message.id = id;
    message.type = REPLY;
    message.exp.arg1 = result;

    write(SOCKETD, & message, sizeof(message));
}

int main(int argc, char *argv[])
{

    if (argc != 4)
        perror("Wrong number of arg\n");

    signal(SIGINT, handle_signal);
    atexit(clean_up);

    NAME = argv[1];
    MODE = argv[2];

    if (strcmp(MODE, "local") == 0)
    {
        SOCKET_PATH = argv[3];
        SOCKETD = init_local_socket();
    }
    else if (strcmp(MODE, "inet") == 0)
    {
        SERVER_ADDRESS = argv[3];
        PORT = (unsigned short)strtoul(argv[4], NULL, 0);
        SOCKETD = init_inet_socket();
    }

    login_client();

    while (1)
    {
        msg message;
        ssize_t bytes = recv(SOCKETD, &message, sizeof(message), MSG_WAITALL);

        if (bytes == 0)
        {
            printf("Disconnect 500 internal server error\n");
            fflush(stdout);
            exit(0);
        }

        switch (message.type)
        {
        case PING:
            printf("Pong!\n");
            write(SOCKETD, &message, sizeof(message));
            break;
        case TASK:
            printf("Client received task\n");
            reply(message.exp, message.id);
            break;
        case LOG:
            printf("Client name is occupied!\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}