#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <fcntl.h>           
#include <sys/stat.h>
#include <mqueue.h>

#include "communication.h"

mqd_t PRIVATE_QUEUE;
mqd_t SERVER_QUEUE = -1;
int CLIENT_ID;

char path[10];

struct mq_attr posix_attr;

void handler_SIGINT(int signo);
int create_queue(char *path, int ID);
void register_client(int key);
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

    pid_t pid = getpid();

    sprintf(path, "/%d", pid);

    
    posix_attr.mq_maxmsg = MAX_MESSAGE_QUEUE_SIZE;
    posix_attr.mq_msgsize = sizeof(struct msgBuf);


    SERVER_QUEUE = mq_open(serverPath, O_WRONLY);
    if(SERVER_QUEUE == -1)
        perror("Fail to open queue \n");


    PRIVATE_QUEUE = mq_open(path, O_RDONLY | O_CREAT, 0666, &posix_attr);

    register_client(getpid());


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
    exit(EXIT_SUCCESS);
}


void register_client(int key)
{
    int sizeOfMessage = sizeof(struct msgBuf);
    struct msgBuf *message = malloc(sizeOfMessage);

    message->mtype = REGISTER;
    message->pid = getpid();

    

    sprintf(message->text, "%d", key);

    

    if (mq_send(SERVER_QUEUE,(char*) message, sizeOfMessage, 1) == -1)
        perror("Client: REGISTER request failed\n");


    struct msgBuf *receivedMessage = malloc(sizeOfMessage);


    if (mq_receive(PRIVATE_QUEUE,(char*) receivedMessage, sizeOfMessage, NULL) == -1)
        perror("Client: catching LOGIN response failed\n");

  
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

    if (mq_send(queueDesc,(char*) sendBack, sizeOfMessage, 1) == -1)
        perror("Client: REGISTER request failed\n");
}

struct msgBuf *receive_message()
{
    int sizeOfMessage = sizeof(struct msgBuf);
    struct msgBuf *receivedMessage = malloc(sizeOfMessage);

    if (mq_receive(PRIVATE_QUEUE,(char*) receivedMessage, sizeOfMessage, NULL) == -1)
        perror("Client: catching LOGIN response failed\n");

    return receivedMessage;
}

void close_queue()
{
     mq_close(PRIVATE_QUEUE);
    mq_unlink(path);
}

void call_action(char *line)
{
    char *temp = malloc(strlen(line));
    strcpy(temp, line);
    char *actionStr = strtok(temp, " ");

    char message[MAX_TEXT_SIZE];

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