#ifndef COMMON_H
#define COMMON_H

#include <sys/epoll.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SUM 0
#define SUB 1
#define MUL 2
#define DIV 3

#define TASK  1
#define REPLY 2
#define PING 3
#define LOG 4

#define MAX_NAME_LEN 30
#define MAX_PATH 126
#define MAX_EVENTS 5
#define MAX_CLIENTS 10

#define READ_SIZE     2

typedef struct client {
  char name[MAX_NAME_LEN];
  struct sockaddr addr;
  socklen_t addr_size;
  int type;
  int fd;
  int pings;
}client;

typedef struct expression {
  int type;
  int arg1;
  int arg2;
}expression;

typedef struct msg {
  int type;
  int id;
  char name[MAX_NAME_LEN];
  expression exp;
}msg;

#endif