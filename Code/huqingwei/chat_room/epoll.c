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

Node *head = NULL;  //创建头指针

void connect_mysql(MYSQL *mysql){  //连接数据库
    if(NULL == mysql_init(mysql)){
        printf("mysql_init(): %s\n", mysql_error(mysql));
        exit(1);
    }
    if(NULL == mysql_real_connect(mysql, "localhost", "root", "hqw479.#", "chat_room", 0, NULL, 0)){
        printf("mysql_real_connect(): %s\n", mysql_error(mysql));
    }
}

MYSQL_RES *get_mysql_result(MYSQL *mysql, char *str){  //获得结果集
    printf("str: %s\n", str);
    int ret = mysql_real_query(mysql, str, strlen(str));
    if(ret){
        printf("line: %d\n", __LINE__-2);
        printf("mysql_real_query() error\n");
        mysql_close(mysql);
        exit(1);
    }

    MYSQL_RES *res = NULL;
    res = mysql_store_result(mysql);
    if(!ret){
        printf("line: %d\n", __LINE__-2);
        printf("mysql_store_result() error\n");
        mysql_close(mysql);
        exit(1);
    }

    return res;
}

void charu_list(Node *phead, user_date date);    //尾插法
void chuli_request(int conn_fd, MSG *message);   //处理客户端请求
void reg(int client_fd, MSG *message);           //注册
void login(int client_fd, MSG *message);         //登录
void all_friends(int client_fd, MSG *message);   //查看所有好友
void online_friend(int client_fd, MSG *message); //查看在线好友

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
    //设置端口复用
    int option_value = 1;
    int length = sizeof(option_value);
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &option_value, length);
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
                printf("client IP: %s\t\tprot: %d----连接成功\n", 
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
                //if(message.cmd == 1)

                //写数据，处理客户端的请求
                chuli_request(fd, &message);
                //printf("---cmd = %d\n", message.cmd);
                //send(fd, &message, sizeof(MSG), 0);
            }
        }
    }
    return 0;
}

void charu_list(Node *phead, user_date date) {
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

void chuli_request(int conn_fd, MSG *message) {
    switch(message->type) {
    case 'r':
        reg(conn_fd, message);
        send(conn_fd, message, sizeof(MSG), 0);
        break;
    case 'l':
        //printf("name:%s\n", message->user_infor.name);
        login(conn_fd, message);
        send(conn_fd, message, sizeof(MSG), 0);
        break;
    case 'a':
        all_friends(conn_fd, message);
        break;
    case 'o':
        online_friend(conn_fd, message);
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
    if(NULL == mysql_real_connect(&mysql, "localhost", "root", "hqw479.#", "chat_room", 0, NULL, 0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        printf("line: %d\n", __LINE__-2);
        exit(1);
    }
    char str[200];
    sprintf(str, "insert into user values(NULL, '%s','%s','%s','%s')",
            message->user_infor.name, message->user_infor.nickname, message->user_infor.sex, message->user_infor.password);
    //printf("%s\n", str);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret == 0) {
        printf("%s 注册成功\n", message->user_infor.name);
        message->cmd = 1001; //注册成功
    }
    else {
        printf("%s 注册失败——>数据库中已有该用户\n", message->user_infor.name);
        message->cmd = -1; //执行失败，数据库中已有该用户
    }
    printf("type = %c\n", message->type);

    //send(client_fd, &message, sizeof(message), 0);

    //关闭数据库
    mysql_close(&mysql);
}

void login(int client_fd, MSG *message) {
    user_date date;
    MYSQL mysql;
    if(NULL == mysql_init(&mysql)) {
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        exit(1);
    }

    if(NULL == mysql_real_connect(&mysql, "localhost", "root", "hqw479.#", "chat_room", 0, NULL, 0)) {
        printf("mysql_real_connect(): %s", mysql_error(&mysql));
        printf("line: %d", __LINE__-2);
        exit(1);
    }
    
    char str[200];
    sprintf(str, "select * from user where u_name = '%s' and u_password = '%s'",
             message->user_infor.name, message->user_infor.password);

    int ret = mysql_real_query(&mysql, str, strlen(str));
    //printf("ret = %d\n", ret);
    /*if(ret == 0) {
        printf("%s 登陆成功\n", message->user_infor.name);
        message->cmd = 1002;
        strcpy(date.name, message->user_infor.name);
        date.socket = client_fd;
        //insert_list(phead, date);
    }
    else {
        printf("%s 登录失败\n", message->user_infor.name);
        message->cmd = -2;
    }*/
    if(ret){
        mysql_close(&mysql);
        return;
    }

    //获得结果集
    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(!mysql_res){
        mysql_close(&mysql);
        return;
    }

    int rows, fields;
    rows = mysql_num_rows(mysql_res); //获得行数
    fields = mysql_num_fields(mysql_res);  //获得列数
    if(rows == 0){
        printf("%s 登录失败\n", message->user_infor.name);
        message->cmd = -2;
    }
    else{
        printf("%s 登陆成功\n", message->user_infor.name);
        message->cmd = 1002;
        strcpy(date.name, message->user_infor.name);
        date.socket = client_fd;
        charu_list(head, date);
    }

    //释放结果集
    mysql_free_result(mysql_res);
    send(client_fd, &message, sizeof(message), 0);
    
    mysql_close(&mysql);
}

void all_friends(int client_fd, MSG *message){
    MYSQL mysql;
    if(NULL == mysql_init(&mysql)){
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        exit(1);
    }
    if(NULL == mysql_real_connect(&mysql, "localhost", "root", "hqw479.#", "chat_room", 0, NULL, 0)){
        printf("mysql_real_connect: %s\n", mysql_error(&mysql));
        exit(1);
    }
    char str[200];
    sprintf(str, "select * from friend where f_username = '%s'", message->user_infor.name);
    printf("%s", str);

    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("mysql_real_query() error\n");
        mysql_close(&mysql);
        return;
    }

    //获得结果集
    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(!mysql_res){
        printf("mysql_store_result() error\n");
        mysql_close(&mysql);
        return;
    }

    int rows, fields;
    MYSQL_ROW row;
    //MYSQL_FIELD *field;
    
    rows = mysql_num_rows(mysql_res);
    fields = mysql_num_fields(mysql_res);
    
    message->friend_num = rows;
    //给客户端发送好友个数
    //send(client_fd, message, sizeof(MSG), 0);
    //用户没有朋友，退出该函数
    if(rows == 0){
        message->friend_num = 0;
        mysql_free_result(mysql_res);
        send(client_fd, message, sizeof(MSG), 0);
        return;
    }
    while(row = mysql_fetch_row(mysql_res)){
        strcpy(message->friend_name, row[1]);
        //printf("friend: %s\n", message->friend_name);
        send(client_fd, message, sizeof(MSG), 0);
    }
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}

void online_friend(int client_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    //connect_mysql(&mysql);
    mysql_init(&mysql);
    mysql_real_connect(&mysql, "localhost", "root", "hqw479.#", "chat_room", 0, NULL, 0);
    
    char str[200];
    sprintf(str, "select * from friend where f_username = '%s'", message->user_infor.name);
    printf("%s\n", str);


    //获得结果集
    //MYSQL_RES *mysql_res;
    //mysql_res = get_mysql_result(&mysql, str);

    
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d\n", __LINE__-2);
        printf("mysql_real_query() error\n");
        mysql_close(&mysql);
        exit(1);
    }

    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(!ret){
        printf("line: %d\n", __LINE__-2);
        printf("mysql_store_result() error\n");
        mysql_close(&mysql);
        exit(1);
    }

    int rows;
    MYSQL_ROW row;
    rows = mysql_num_rows(mysql_res);
    if(rows == 0){
        message->friend_num = 0;
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        send(client_fd, message, sizeof(MSG), 0);
        return;
    }
    Node *current;
    int flag;
    while(row = mysql_fetch_row(mysql_res)){
        flag = 0;
        current = head;
        while(current != NULL){
            if(strcmp(message->user_infor.name, row[2])){
                flag = 1;
                break;
            }
            current = current->next;
        }
        if(flag == 1){
            strcpy(message->friend_name, row[2]);
            send(client_fd, message, sizeof(MSG), 0);
        }
    }
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}
