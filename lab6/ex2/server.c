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

mqd_t QUEUE_ID;
int IS_SERVER_ON = 1;
struct clientsInfo clients[MAX_CLIENTS];

struct mq_attr posix_attr;

char path[10];

void handler_SIGINT(int signo);
int create_queue(int key);
void process_message(struct msgBuf *message);
void mirror_action(struct msgBuf *message);
void calc_action(struct msgBuf *message);
void time_action(struct msgBuf *message);
void end_action(struct msgBuf *message);
void register_action(struct msgBuf *message);
int register_client(int client_key, pid_t pid);
void quit_action(struct msgBuf *message);
char *reverseConstString(char const *str);
int get_id();
void init_clients_info();
void send_message(int queueDesc, int type, char *text);
void close_queue();

int main(int argc, char *argv[])
{
    if (signal(SIGINT, handler_SIGINT) == SIG_ERR)
    {
        perror("Faild to set handler for SIGINT \n");
    }

    if (atexit(close_queue) == -1)
    {
        perror("Faild to set atexit function \n");
    }

    sprintf(path, "/%d", getpid());

    //char *home = getenv("HOME");

    //key_t publicKey = ftok(home, PROJECT_ID);

    struct mq_attr current_state;

    
    posix_attr.mq_maxmsg = MAX_MESSAGE_QUEUE_SIZE;
    posix_attr.mq_msgsize = sizeof(struct msgBuf);

    QUEUE_ID = mq_open(serverPath, O_RDONLY | O_CREAT, 0666, &posix_attr);

    struct msgBuf message;
    int sizeOfmessage = sizeof(struct msgBuf);

    init_clients_info();

    while (1)
    {
        if (IS_SERVER_ON == 0)
        {

            if (mq_getattr(QUEUE_ID, &current_state) == -1)
                perror("server: fail of getting current state of queue\n");

            if (current_state.mq_curmsgs == 0)
                break;
        }

        if (mq_receive(QUEUE_ID,(char*) &message, sizeOfmessage, &posix_attr) == -1)
        {
            perror("Fail to receive message\n");
        }

        process_message(&message);
    }

    exit(EXIT_SUCCESS);
}

int create_queue(int key)
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

void process_message(struct msgBuf *message)
{
    if (message == NULL)
        return;

    switch (message->mtype)
    {
    case MIRROR:
        mirror_action(message);
        break;
    case CALC:
        calc_action(message);
        break;
    case TIME:
        time_action(message);
        break;
    case END:
        end_action(message);
        break;
    case REGISTER:
        register_action(message);
        break;
    case QUIT:
        quit_action(message);
        break;
    default:
        break;
    }
}

void mirror_action(struct msgBuf *message)
{
    char *reverse = reverseConstString(message->text);

    int client_id;
    if ((client_id = get_id_by_pid(message->pid)) == -1)
        perror("Server: unregitser client \n");

    //send back
    send_message(clients[client_id].clientQueueDesc, MIRROR, reverse);
}

void calc_action(struct msgBuf *message)
{
    char result[MAX_TEXT_SIZE];

    char cmd[4096];
    sprintf(cmd, "echo '%s' | bc", message->text);
    FILE *calc = popen(cmd, "r");
    fgets(result, MAX_TEXT_SIZE, calc);
    pclose(calc);

    int client_id;
    if ((client_id = get_id_by_pid(message->pid)) == -1)
        perror("Server: unregitser client \n");
    //send back
    send_message(clients[client_id].clientQueueDesc, CALC, result);
}

void time_action(struct msgBuf *message)
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char serverTime[MAX_TEXT_SIZE];
    sprintf(serverTime, "%s", asctime(timeinfo));

    int client_id;
    if ((client_id = get_id_by_pid(message->pid)) == -1)
        perror("Server: unregitser client \n");
    //send back
    send_message(clients[client_id].clientQueueDesc, TIME, serverTime);
}

void end_action(struct msgBuf *message)
{
    IS_SERVER_ON = 0;
}

void register_action(struct msgBuf *message)
{
    printf("received register signal\n");

    int client_key;

    if (sscanf(message->text, "%d", &client_key) < 0)
        perror("Server: fail to read regiter message\n");

    int client_id = register_client(client_key, message->pid);

    char *text = calloc(MAX_TEXT_SIZE, sizeof(char));
    sprintf(text, "%d", client_id);

    send_message(clients[client_id].clientQueueDesc, REGISTER, text);
}

void quit_action(struct msgBuf *message)
{
    int client_id = get_id_by_pid(message->pid);

    pid_t pid = clients[client_id].pid;

    clients[client_id].used = 0;
    clients[client_id].pid = 0;

    msgctl(clients[client_id].clientQueueDesc, IPC_RMID, &posix_attr);

    clients[client_id].clientQueueDesc = 0;

    kill(pid, SIGKILL);
}

//HELPERS

void handler_SIGINT(int signo)
{
    printf("Received ctr+c \n");
    //msgctl(QUEUE_ID, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}

char *reverseConstString(char const *str)
{
    int start, end, len;
    char temp, *ptr = NULL;

    len = strlen(str);

    ptr = (char *)malloc(sizeof(char) * (len + 1));

    ptr = strcpy(ptr, str);

    for (start = 0, end = len - 1; start <= end; start++, end--)
    {
        temp = ptr[start];
        ptr[start] = ptr[end];
        ptr[end] = temp;
    }

    return ptr;
}

void init_clients_info()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].used = 0;
        clients[i].id = i;
    }
}

int get_id()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].used == 0)
        {
            clients[i].used = 1;
            return clients[i].id;
        }
    }

    return -1;
}

int register_client(int client_key, pid_t pid)
{
    int client_id = get_id();

    if (client_id == -1)
        perror("Server: Cannot regiter more clients \n");

    int client_queue = msgget(client_key, 0);
    clients[client_id].clientQueueDesc = client_queue;
    clients[client_id].pid = pid;

    return client_id;
}

void send_message(mqd_t mqdes, int type, char *text)
{
    int sizeOfMessage = sizeof(struct msgBuf);
    struct msgBuf *sendBack = malloc(sizeOfMessage);

    sendBack->mtype = type;
    sendBack->pid = getpid();
    strcpy(sendBack->text, text);

    if (mq_send(mqdes, sendBack, sizeOfMessage, 1) == -1)
        ferror("Server: request failed\n");
}

int get_id_by_pid(pid_t pid)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].used == 1 && clients[i].pid == pid)
        {
            return clients[i].id;
        }
    }

    return -1;
}

void close_queue()
{
    mq_close(QUEUE_ID);
    mq_unlink(path);
}