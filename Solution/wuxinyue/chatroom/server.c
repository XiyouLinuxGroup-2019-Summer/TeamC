/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 21时05分31秒
 ************************************************************************/

#include "server.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>

#include "define.h"
#include "socket.h"
#include "proto.h"
#include "log.h"

int server_init(server_t *server)
{
    server->stop = 0;
    server->listen_fd = 0;
    bzero(server->users, sizeof(server->users));
    server->epoll_fd = epoll_create(1024);
    if (server->epoll_fd < 0) {
        log_error("server create epoll failed, exit");
        abort();
    }
    return CR_OK;
}

int server_start(server_t *server)
{
    server->listen_fd = tcp_listen(LISTEN_ADDR, LISTEN_PORT, LISTEN_BACKLOG);
    if (server->listen_fd < 0) {
        log_error("server start error, exit");
        abort();
    }
    log_info("server started, listen on %s:%d", LISTEN_ADDR, LISTEN_PORT);
    struct epoll_event event;
    event.data.fd = server->listen_fd;
    event.events = EPOLLIN;
    int  ret = epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, server->listen_fd, &event);
    if (ret < 0) {
        log_error("server add listen fd to epoll failed, exit");
        abort();
    }
    return CR_OK;
}

int read_from_fd(int fd, char *buff, int len)
{
    int nread = read(fd, buff, len);
    if (nread == 0 || (nread == -1 && errno != EINTR && errno != EAGAIN)) {
        log_warn("client fd read error, close it, error: %s", strerror(errno));
        return -1;
    }
    if (nread != len) {
        // TODO
        log_warn("not read enough, give up...");
        return -1;
    }

    return nread;
}

int write_to_fd(int fd, const char *buff, int len)
{
    int nwritten = write(fd, buff, len);
    if (nwritten == 0 || (nwritten == -1 && errno != EINTR && errno != EAGAIN)) {
        close(fd);
    }
    if (nwritten != len) {
        log_warn("nwritten != nread, but...give up...");
        return -1;
    }

    return nwritten;
}

// 定义一个数组 存放的为函数指针
typedef proto_head_t *(*packet_process_ft)(proto_head_t *);

proto_head_t *process_register(proto_head_t *req)
{
    request_register_t *request = (request_register_t *)req;
    // 强行错误输出
    fprintf(stderr, "want register, username: %s, password: %s\n", request->username, request->password);
    
    if (strcmp(request->username, "zhangxue") == 0) {
        fprintf(stderr, "不让 zhangxue 注册\n");
        return (proto_head_t *)create_response_status(1, "user exist!");
    }
    // 检查此用户名是否已经注册 
    // TODO
    // do data operator...
    // 进行数据库操作 如果没有被注册过 注册成功
    return (proto_head_t *)create_response_status(0, "success");
}

proto_head_t *process_user_request(proto_head_t *req) {
    // 数组下标为0 执行注册 注意static
    static packet_process_ft process_funcs[] = {
        process_register,
       // process_login,
        // other ...
    };
     int index = req->type - PROTO_TYPE_BASE;
    if (index < 0 || index >= sizeof(process_funcs) / sizeof(process_funcs[0])) {
        log_error("type cannot process: %d", req->type);
        return NULL;
    }
    // 执行数组下标为index的函数
    return process_funcs[index](req);
}


void close_user_fd(server_t *server, int user_fd)
{
    epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, user_fd, NULL);
    close(user_fd);
}

static void process_user_send_data(int fd, server_t *server)
{
    char buffer[1024];
    // 读协议头数据
    int nread = read_from_fd(fd, buffer, sizeof(proto_head_t));
    if (nread < 0) {
        close_user_fd(server, fd);
        return;
    }
    proto_head_t *req = (proto_head_t *)buffer;
    // 如果不是遵循本协议 退出
    if (req->magic != CR_PROTO_HEAD) {
        log_warn("wrong proto head, close it");
        close_user_fd(server, fd);
        return;
    }
    // 收发来的结构体 因为协议头已经接收
    int want_read_len = req->length - sizeof(proto_head_t);
    nread = read_from_fd(fd, buffer + sizeof(proto_head_t), want_read_len);
    if (nread < 0) {
        close_user_fd(server, fd);
        return;
    }
    // 执行函数
    proto_head_t *resp = process_user_request(req);
    
    if (!resp) {
        log_warn("emmmm, check your code, close it");
        close_user_fd(server, fd);
        return;
    }
    write_to_fd(fd, (char *)resp, resp->length);
}


static void accept_new_connection(server_t *server)
{
    char addr[36];
    int port;

    int client_fd = tcp_accept(server->listen_fd, addr, sizeof(addr), &port);   
    if (client_fd < 0) {
        log_error("accept listen fd failed");
        abort();
    }
    log_info("new connection: %s:%d, fd: %d", addr, port, client_fd);
    struct epoll_event event;
    event.data.fd = client_fd;
    event.events = EPOLLIN;
    int ret = epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
    if (ret < 0) {
        log_error("epoll_ctl add client fd read event failed");
        abort();
    }
}

int server_run(server_t *server)
{
    struct epoll_event *events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAX_USER_COUNT);
    while (!server->stop) {
        int nevents = epoll_wait(server->epoll_fd, events, MAX_USER_COUNT, -1);
        if (nevents < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }
            log_errno_error("epoll_wait return failed");
            abort();
        }
        // ignore write event, but...
        for (int i = 0; i < nevents; ++i) {
            int fd = events[i].data.fd;
            if (events[i].events == EPOLLIN) {
                if (fd == server->listen_fd) {
                    accept_new_connection(server);    
                } else {
                    process_user_send_data(fd, server);
                }
            } else { 
                log_warn("fd[%d] return a error, close it", fd);
                epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
            }
        }
    }
    free(events);

    return CR_OK;
}

void server_stop(server_t *server)
{
    server->stop = 1;
}

void server_destory(server_t *server)
{
    close(server->listen_fd);
    close(server->epoll_fd);
}

static server_t server;

void signal_handle(int signal)
{
    (void)signal;
    log_warn("recv sigint, exit now...");
    server_stop(&server);
}

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handle);

    server_init(&server);
    server_start(&server);

    server_run(&server);

    server_destory(&server);

    return EXIT_SUCCESS;
}


