#include "common.h"

int INET;
int LOCAL;
int EPOLLD;
char* SOCKET_NAME;
pthread_mutex_t MUTEX;
pthread_t RESOLVE_THREAD;
pthread_t PING_THREAD;
client clients[MAX_CLIENTS];


void handle_signal(int signum)
{
    printf("Received SIGINT closing server\n");
    exit(EXIT_SUCCESS);
}

void clean() {
    pthread_mutex_destroy(&MUTEX);
    close(EPOLLD);
    close(INET);
    close(LOCAL);
    remove(SOCKET_NAME);
}

void init_INET_socket(unsigned const short port) {
    INET = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(INET, (struct sockaddr*) &(addr), sizeof(addr));
    listen(INET, 10);
    
}

void init_LOCAL_socket(const char* name) {
    LOCAL = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, name);

    bind(LOCAL, (struct sockaddr*) &(addr), sizeof(addr));
    listen(LOCAL, 10);
}

void epoll_init() {
    EPOLLD = epoll_create1(0);
    struct epoll_event e;

    e.events = EPOLLIN | EPOLLET;
    e.data.fd = INET;
    epoll_ctl(EPOLLD, EPOLL_CTL_ADD, INET, &e);

    e.events = EPOLLIN | EPOLLET;
    e.data.fd = LOCAL;
    epoll_ctl(EPOLLD, EPOLL_CTL_ADD, LOCAL, &e);
}

void add_client(struct epoll_event e) {
    pthread_mutex_lock(&MUTEX);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].pings == -1) {
            clients[i].pings = 0;
            struct sockaddr new_addr;
            socklen_t new_addr_len = sizeof(new_addr);
            clients[i].fd = accept(e.data.fd, &new_addr, &new_addr_len);

            struct epoll_event e;
            e.events = EPOLLIN | EPOLLET;
            e.data.fd = clients[i].fd;

            if (epoll_ctl(EPOLLD, EPOLL_CTL_ADD, clients[i].fd, &e) < 0) {
                printf("cannot create EPOLLD for client %d\n", e.data.fd);
                fflush(stdout);
                clients[i].pings = -1;
            }
            pthread_mutex_unlock(&MUTEX);
            return;
        }
    }
    pthread_mutex_unlock(&MUTEX);
}


int resolve_mess(struct epoll_event event) {
    msg ms;
    ssize_t bytes = read(event.data.fd, &ms, sizeof(ms));

    if (bytes == 0) {
        printf("closeing connection %d", event.data.fd);
        close_connection(event);
        return 0;
    } else {
        switch (ms.type) {
            case LOG:
            {
                pthread_mutex_lock(&MUTEX);
                for (int i = 0; i < MAX_CLIENTS; ++i) {
                    if (clients[i].pings >= 0 && strcmp(ms.name, clients[i].name) == 0) {
                        pthread_mutex_unlock(&MUTEX);
                        write(event.data.fd, &ms, sizeof(ms));
                        close_connection(event);
                        return -1;
                    }
                }
                for (int i = 0; i < MAX_CLIENTS; ++i) {
                    if (clients[i].pings >= 0 && event.data.fd == clients[i].fd) {
                        strcpy(clients[i].name, ms.name);
                        pthread_mutex_unlock(&MUTEX);
                        break;
                    }
                }
                break;
            }
            case REPLY:
            {
                printf("result of exp num: %d is: %d \n", ms.id, ms.exp.arg1);
                return 0;
            }
            case PING:
            {
                pthread_mutex_lock(&MUTEX);
                clients[ms.id].pings = 0;
                pthread_mutex_unlock(&MUTEX);
            }
        }
    }
}

void* resolve_calls() {
    for (int i = 0; i < MAX_CLIENTS; ++i) clients[i].pings = -1;
    size_t bytes_read;
    char read_buffer[READ_SIZE + 1];
    int running = 1;
    struct epoll_event events[MAX_EVENTS];

    while (running) {
        int event_count = epoll_wait(EPOLLD, events, MAX_EVENTS, -1);
        for (int i = 0; i < event_count; ++i) {
            if (events[i].data.fd == LOCAL || events[i].data.fd == INET) {
                add_client(events[i]);
            } else {
                resolve_mess(events[i]);
            }
        }
    }
}


void* event(void* args) {
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i].pings = -1;
    struct epoll_event e[MAX_EVENTS];

    while (1) {
        int counter = epoll_wait(EPOLLD, e, MAX_EVENTS, -1);
        for (int i = 0; i < counter; i++) {
            if (e[i].data.fd == LOCAL) {
                printf("LOCAL client is waiting to be added.\n");
                add_client(e[i]);
            } else if (e[i].data.fd == INET) {
                printf("INET client is waiting to be added.\n");
                add_client(e[i]);
            } else {
                resolve_calls(e[i]);
            }
        }
    }
}

void close_connection(struct epoll_event event) {
    pthread_mutex_lock(&MUTEX);
    shutdown(event.data.fd, SHUT_RDWR);
    close(event.data.fd);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].pings >= 0 && event.data.fd == clients[i].fd) {
            clients[i].pings = -1;
            for (int j = 0; j < MAX_NAME_LEN; ++j) clients[i].name[j] = 0;
        }
    }
    pthread_mutex_unlock(&MUTEX);
}

void console()
{
     int z = 0;
    while (1) {
        char c, s;
        int arg1, arg2;
        printf("%d> ", z);

        int res = scanf("%d %c %d", &arg1, &s, &arg2);
        if (res != 3) {
            while((c = getchar()) != '\n' && c != EOF);
            printf("Wrong number of args! \n");
            continue;
        }
        msg m;
        m.exp.arg1 = arg1;
        m.exp.arg2 = arg2;
        m.id = z++;
        m.type = TASK;

        switch (s) {
            case '+': m.exp.type = SUM; break;
            case '-': m.exp.type = SUB; break;
            case '*': m.exp.type = MUL; break;
            case '/': m.exp.type = DIV; break;
        }
        int done = 0;
        while (!done) {
            pthread_mutex_lock(&MUTEX);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].pings >= 0) {
                    write(clients[i].fd, &m, sizeof(m));
                    sleep(2);
                    done = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&MUTEX);
        }
    }
}

void* ping() {
    while (1) {
        msg ms;
        ms.type = PING;
        pthread_mutex_lock( & MUTEX);
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i].pings == 0) {
                clients[i].pings = 1;
                ms.id = i;
                write(clients[i].fd, &ms, sizeof(ms));
            }
        }
        pthread_mutex_unlock(&MUTEX);
        sleep(10);
    }

}



int main(int argc, char **argv)
{

    if (argc != 3)
        perror("Wrong number of agr\n");

    signal(SIGINT, handle_signal);
    atexit(clean);

    pthread_mutex_init(&MUTEX, NULL);

    unsigned short port = (unsigned short) strtoul(argv[1], NULL, 0);
    SOCKET_NAME = argv[2];

    init_INET_socket(port);
    init_LOCAL_socket(SOCKET_NAME);
    epoll_init();

    pthread_create(&RESOLVE_THREAD, NULL, resolve_calls, NULL);
    pthread_create(&PING_THREAD, NULL, ping, NULL);

    console();

    pthread_cancel(RESOLVE_THREAD);
    pthread_cancel(PING_THREAD);
    return 0;
}
