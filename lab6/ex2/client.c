#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include <fcntl.h>           
#include <sys/stat.h>
#include <mqueue.h>

#include "communication.h"

mqd_t PRIVATE_QUEUE;
mqd_t SERVER_QUEUE;
int CLIENT_ID;

char path[10];

struct mq_attr posix_attr;

void handler_SIGINT(int signo);
int create_queue(char *path, int ID);
void register_client(char* key);
void mirror_request(char *text);
struct msgBuf *receive_message();

void send_message(int queueDesc, int type, char *text);
void calc_request(char *text);
void time_request();
void end_request();
void close_queue();
void quit_request();
void call_action(char *line);

int main(int argc, char *argv[])
{
    if (signal(SIGINT, handler_SIGINT) == SIG_ERR)
    {
        perror("Faild to set handler for SIGINT");
    }

    atexit(close_queue);

    sprintf(path, "/%d", getpid());

    
    posix_attr.mq_maxmsg = MAX_MESSAGE_QUEUE_SIZE;
    posix_attr.mq_msgsize = sizeof(struct msgBuf);

    //char *home = getenv("HOME");

    //SERVER_QUEUE = create_queue(home, PROJECT_ID);
    SERVER_QUEUE = mq_open(serverPath, O_WRONLY);

    //int privateKey = ftok(home, getpid());

    //PRIVATE_QUEUE = create_private_queue(privateKey);
    PRIVATE_QUEUE = mq_open(path, O_RDONLY | O_CREAT, 0666, &posix_attr);

    register_client(path);

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1)
    {
        call_action(line);
    }

    return 0;
}

void handler_SIGINT(int signo)
{
    printf("Received ctr+c \n");
    //msgctl(PRIVATE_QUEUE, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}

/*
int create_queue(char *path, int ID)
{
    int key = ftok(path, ID);

    int queue = msgget(key, IPC_PRIVATE);

    if (key != -1)
    {
        return queue;
    }
    else
    {
        perror("Fial to crete queue");
    }
}

int create_private_queue(int key)
{
    int queue = msgget(key, IPC_CREAT | 0666);

    if (key != -1)
    {
        return queue;
    }
    else
    {
        perror("Fial to crete queue");
    }
}

*/

void register_client(char* key)
{
    int sizeOfMessage = sizeof(struct msgBuf);
    struct msgBuf *message = malloc(sizeOfMessage);

    message->mtype = REGISTER;
    message->pid = getpid();
    sprintf(message->text, "%d", key);

    if (mq_send(SERVER_QUEUE, message, sizeOfMessage, 1) == -1)
        ferror("Client: REGISTER request failed\n");

    struct msgBuf *receivedMessage = malloc(sizeOfMessage);

    if (mq_receive(PRIVATE_QUEUE, receivedMessage, sizeOfMessage, &posix_attr) == -1)
        ferror("Client: catching LOGIN response failed\n");

    int client_id;

    printf("Registered client with key: %s \n", receivedMessage->text);

    free(message);
    free(receivedMessage);
}

void mirror_request(char *text)
{
    send_message(SERVER_QUEUE, MIRROR, text);

    struct msgBuf *receivedMessage = receive_message();

    printf("%s\n", receivedMessage->text);
}

void calc_request(char *text)
{
    send_message(SERVER_QUEUE, CALC, text);

    struct msgBuf *receivedMessage = receive_message();

    printf("%s", receivedMessage->text);
}

void time_request()
{
    send_message(SERVER_QUEUE, TIME, NULL);

    struct msgBuf *receivedMessage = receive_message();

    printf("%s", receivedMessage->text);
}

void end_request()
{
    send_message(SERVER_QUEUE, END, NULL);

    exit(EXIT_SUCCESS);
}

void quit_request()
{
    send_message(SERVER_QUEUE, QUIT, NULL);
}

//HELPERS

void send_message(int queueDesc, int type, char *text)
{
    int sizeOfMessage = sizeof(struct msgBuf);
    struct msgBuf *sendBack = malloc(sizeOfMessage);

    sendBack->mtype = type;
    sendBack->pid = getpid();

    if (text != NULL)
        strcpy(sendBack->text, text);

    if (mq_send(queueDesc, sendBack, sizeOfMessage, 1) == -1)
        ferror("Client: REGISTER request failed\n");
}

struct msgBuf *receive_message()
{
    int sizeOfMessage = sizeof(struct msgBuf);
    struct msgBuf *receivedMessage = malloc(sizeOfMessage);

    if (mq_receive(PRIVATE_QUEUE, receivedMessage, sizeOfMessage, &posix_attr) == -1)
        ferror("Client: catching LOGIN response failed\n");

    return receivedMessage;
}

void close_queue()
{
    //msgctl(PRIVATE_QUEUE, IPC_RMID, NULL);
     mq_close(PRIVATE_QUEUE);
    mq_unlink(path);
}

void call_action(char *line)
{
    char *temp = malloc(strlen(line));
    strcpy(temp, line);
    char *actionStr = strtok(temp, " ");

    char *message[MAX_TEXT_SIZE];

    char *text = strchr(line, ' ');
    if (text != NULL)
        text = text + 1;
    for (int i = 0; i < MAX_TEXT_SIZE; i++)
        message[i] = '\0';
    if (text != NULL)
        strcpy(message, text);

    if (strncmp(actionStr, "mirror", 6) == 0)
    {
        mirror_request(message);
    }
    else if (strncmp(actionStr, "calc", 4) == 0)
    {
        calc_request(message);
    }
    else if (strncmp(actionStr, "time", 4) == 0)
    {
        time_request();
    }
    else if (strncmp(actionStr, "end", 3) == 0)
    {
        end_request();
    }
    else if (strncmp(actionStr, "quit", 4) == 0)
    {
        quit_request();
    }
}