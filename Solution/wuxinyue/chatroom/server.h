/*************************************************************************
	> File Name: server.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 21时04分02秒
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H

#include "proto.h"

#define LISTEN_ADDR "0.0.0.0"
#define LISTEN_BACKLOG 1024

#define MAX_USER_COUNT 1000
#define PROTO_TYPE_BASE 1000

typedef struct user {
    int is_login;
    char username[USERNAME_LEN];
} user_t;

typedef struct server {
    int stop;
    int listen_fd;
    int epoll_fd;
    user_t users[MAX_USER_COUNT];
} server_t;

int server_init(server_t *server);
int server_start(server_t *server);
int server_run(server_t *server);
void server_stop(server_t *server);
void server_destory(server_t *server);

#endif
