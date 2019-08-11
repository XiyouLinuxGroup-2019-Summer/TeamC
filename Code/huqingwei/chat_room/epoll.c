#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <error.h>
#include <mysql/mysql.h>

#include "chat.h"

#define PORT 9876

typedef struct user_date{
    char name[20];
    int socket;
}user_date;

typedef struct node{
    user_date date;
    struct node *next;
}Node;

//创建头指针
Node *phead = NULL;

//尾插法
void insert_list(Node *phead, user_date date);

//处理客户端请求
void chuli_request(int conn_fd, MSG message);

//用户注册
void reg(int client_fd, MSG *message);

int main()
{
    struct sockaddr_in serv_addr;
    socklen_t serv_len = sizeof(serv_addr);

    //创建套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    //初始化服务器 sockaddr_in
    memset(&serv_addr, 0, serv_len);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //监听本机所有IP
    serv_addr.sin_port = htons(PORT); //设置端口
    //绑定IP和端口
    bind(lfd, (struct sockaddr*)&serv_addr, serv_len);

    //设置同事间厅的最大个数
    listen(lfd, 36);
    printf("Start accept ......\n");

    struct sockaddr_in client_addr;
    socklen_t cli_len = sizeof(client_addr);

    //epoll树根节点
    int epfd = epoll_create(3000);
    //存储发生变化的fd对应的信息
    struct epoll_event all[3000];
    //inti
    //监听的lfd挂到树上
    struct epoll_event ev;
    //在ev中init lfd
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    while(1) {
        //委托内核检测事件
        int ret = epoll_wait(epfd, all, 3000, -1);
        //根据ret遍历all数组
        for(int i=0; i<ret; i++) {
            int fd = all[i].data.fd;
            //有新的连接
            if(fd == lfd) {
                //接受连接请求 - accept不阻塞
                int cfd = accept(lfd, (struct sockaddr*)&client_addr, &cli_len);
                //cfd上树
                ev.events = EPOLLIN;
                ev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                //将新连接的客户端信息加入到链表中
                char ip[36];
                printf("client IP: %s\t\tprot: %d\n", 
                        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)),
                        ntohs(client_addr.sin_port));
            }

            //已经连接的客户端有数据发送过来
            else {
                //只处理客户端发送的数据
                if(!all[i].events & EPOLLIN) {
                    continue;
                }

                //读数据
                MSG message;
                int len = recv(fd, &message, sizeof(MSG), 0);

                if(len == 0) {
                    //检测的fd从树上删除
                    char ip[36];
                    printf("client IP: %s\t\tport: %d 已断开连接\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)),
                            ntohs(client_addr.sin_port));
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
                if(message.cmd == 1)

                //写数据，处理客户端的请求
                chuli_request(fd, message);
            }
        }
    }
    return 0;
}

void inster_list(Node *phead, user_date date) {
    Node *current, *node;
    node = (Node*)malloc(sizeof(Node));
    
    //init要插入节点
    node->date = date;
    node->next = NULL;
    
    if(phead == NULL) {
        phead = node;
        return;
    }
    
    //找到尾节点
    current = phead;
    while(current->next != NULL) {
        current = current->next;
    }

    current->next = node;
    return;
}

void chuli_request(int conn_fd, MSG message) {
    switch(message.cmd) {
    case 1:
        reg(conn_fd, &message);
        break;
    }
}

void reg(int client_fd, MSG *message) {
    printf("%s 正在进行注册\n", message->user_infor.name);

    //声明数据库
    MYSQL mysql;
    //init 数据库
    if(NULL == mysql_init(&mysql)) {
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        exit(1);
    }
    
    //连接数据库
    mysql_real_connect(&mysql, "localhost", "root", "hqw479.#", "chati_room", 0, NULL, 0);
    char str[200];
    sprintf(str, "insert into user values(NULL,'%s','%s','%s','%s','%s')",
            message->user_infor.login_id, message->user_infor.name, 
            message->user_infor.nickname, message->user_infor.sex, message->user_infor.password);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret == 0) {
        message->cmd = 1001; //注册成功
    }
    else {
        message->cmd = -1; //执行失败，数据库中已有该用户
    }

    send(client_fd, &message, sizeof(message), 0);

    //关闭数据库
    mysql_close(&mysql);
}
