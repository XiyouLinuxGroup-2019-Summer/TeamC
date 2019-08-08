#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "chat.h"

struct client_infor{
    int fd;
    int epfd;
    struct epoll_event ev;
    struct sockaddr_in client_addr;
    socklen_t sli_len = sizeof(client_addr);
};

void *cmd_client(void *arg);

int main()
{
    //创建监听套接字
    int lfd = listen_socket();
    if(lfd == -1) {
        printf("listen_socket create false\n");
    }
    printf("Start accept ......\n");

    //接受的客户端信息
    //struct sockaddr_in client_addr;
    //socklen_t cli_len = sizeof(client_addr);
    struct client_infor *client_infor;
    client_infor = (struct client_infor*)malloc(sizeof(struct client_infor));
    
    //epoll树根节点
    client_infor->epfd = epoll_create(3000);
    //存储发生变化的fd对应的信息
    struct epoll_event all[3000];
    //init
    //监听的lfd挂到树上
    //struct epoll_event ev;
    client_infor->ev.data.fd = lfd;
    epoll_ctl(client_infor->epfd, EPOLL_CTL_ADD, lfd, &client_infor->ev);

    while(1) {
        //委托内核检测事件
        int ret = epoll_wait(client_infor->epfd, all, 3000, -1);
        if(ret == -1) {
            my_err("epoll_wait", __LINE__-2);
        }
        //根据ret遍历all数组
        for(int i=0; i<ret; i++) {
            client_infor->fd = all[i].data.fd;
            //有新的连接
            if(client_infor->fd == lfd) {
                //接受连接请求 - accept不阻塞
                int cfd = accept(lfd, (struct sockaddr*)&client_infor->client_addr, &client_infor->sli_len);
                //cfd上树
                client_infor->ev.events = EPOLLIN;
                client_infor->ev.data.fd = cfd;
                epoll_ctl(client_infor->epfd, EPOLL_CTL_ADD, cfd, &client_infor->ev);
                char ip[36];
                printf("client IP: %s\t\tport: %d 连接成功\n", 
                       inet_ntop(AF_INET, &client_infor->client_addr.sin_addr.s_addr, ip, sizeof(ip)),
                       ntohs(client_infor->client_addr.sin_port));
            }

            //已经连接的客户有数据发送过来
            else {
                //只处理客户端发送的数据
                if(!all[i].events & EPOLLIN) {
                    continue;
                }

                else {
                    //创建线程处理客户端发送来的请求
                    pthread_t id;
                    pthread_create(&id, NULL, cmd_client, (void*)client_infor);
                    
                }
            }
        }
    }

    return 0;
}

void *cmd_client(void *arg) {
    struct client_infor client_infor;
    MSG message;

    client_infor = *(struct client_infor*)arg;

    int len = recv_message(client_infor.fd, &message);

    if(len == 0) {
        //检测到fd从树上删除
        char ip[36];
        printf("client: %s\t\t%d 已断开连接\n",
               inet_ntop(client_infor.fd, &client_infor.client_addr, ip, sizeof(ip)),
               ntohs(client_infor.client_addr.sin_port));
        epoll_ctl(client_infor.epfd, EPOLL_CTL_DEL, client_infor.fd, NULL);
        epoll_ctl(client_infor.epfd, EPOLL_CTL_DEL, client_infor.fd, NULL);
    }

    //写数据，处理客户端的请求
    send(client_infor.fd, &message, sizeof(MSG), 0);
}

