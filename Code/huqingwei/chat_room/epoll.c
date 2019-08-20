#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <error.h>
#include <mysql/mysql.h>

#include "chat.h"

#define PORT 9876

typedef struct user_date{  //用户结构体
    char name[20];
    int socket;
}user_date;

typedef struct request_node{ //请求结构体，节点
    //MSG request;
    char from_name[20];
    char to_name[20];
    char to_group_name[20];
    char type;  //区别好友请求和群请求
    struct request_node *next;
}request_node;

typedef struct node{  //用户节点
    user_date date;
    struct node *next;
}Node;

Node *head = NULL;  //创建头指针
request_node *head_request = NULL;  //创建请求队列头指针

void my_err(char *err, int line){
    printf("line: %d\n", line);
    perror(err);
    exit(1);
}

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
    if(!res){
        printf("line: %d\n", __LINE__-2);
        printf("mysql_store_result() error\n");
        mysql_close(mysql);
        exit(1);
    }

    return res;
}
//尾插法
//void charu_list(Node *phead, user_date date) {
void charu_list(user_date date){
    Node *current, *new_node;
    new_node = (Node*)malloc(sizeof(Node));
    
    //init要插入节点
    //strcpy(node->date.name, date.name);
    //node->date.socket = date.socket;
    new_node->date = date;
    new_node->next = NULL;
    
    if(head == NULL) {
        //printf("A\n");
        head = new_node;
        return;
    }
    
    //找到尾节点
    current = head;
    while(current->next != NULL) {
        current = current->next;
    }

    current->next = new_node;
    return;
}


//链表节点数
void list_num(){
    Node *current;
    int num = 0;

    current = head;
    while(current != NULL){
        num++;
        printf("name: %s\n", current->date.name);
        current = current->next;
    }

    printf("num = %d\n", num);
}

//删除节点
void del_node(int socket_fd){
    Node *current, *prev;
    
    if(head == NULL){  //如果没有用户在线，跳过该函数
        return;
    }

    prev = head;
    current = head;
    while(current->next != NULL){
        if(current->date.socket == socket_fd){
            break;
        }
        prev = current;
        current = current->next;
    }

    if(current == NULL){
        printf("false\n");
        exit(1);
    }
    
    if(prev == head){
        head = current->next;
        free(current);
        return;
    }

    prev->next = current->next;
    free(current);
    return;
}

//void charu_list(Node *phead, user_date date);    //尾插法
//void charu_list(user_date date);
void chuli_request(int conn_fd, MSG *message);   //处理客户端请求
void reg(int client_fd, MSG *message);           //注册
void login(int client_fd, MSG *message);         //登录
void forget_password(int conn_fd, MSG *message); //忘记密码
void all_friends(int client_fd, MSG *message);   //查看所有好友
void online_friend(int client_fd, MSG *message); //查看在线好友
void private_chat(int client_fd, MSG *message);  //私聊功能
void add_friend(int conn_fd, MSG *message);       //添加好友
void delete_friend(int conn_fd, MSG *message);   //删除好友
void deal_friend_request_y(int conn_fd, MSG *message);  //同意好友申请
void deal_friend_request_n(int conn_fd, MSG *message);  //拒绝好友申请
void create_group(int conn_fd, MSG *message);    //创建群聊
void view_group(int conn_fd, MSG *message);      //查看群
void public_chat(int conn_fd, MSG *message);     //群聊功能
void add_group(int conn_fd, MSG *message);       //申请加群
void quit_group(int conn_fd, MSG *message);      //退出群聊
void deal_group_request_y(int conn_fd, MSG *message);  //同意入群申请
void deal_group_request_n(int conn_fd, MSG *message);  //拒绝入群申请
void send_file(int conn_fd, MSG *message);             //发送文件
char *get_file_name(char *file_path);                  //获得文件名
void view_group_member(int conn_fd, MSG *message);     //查看群成员
void break_group(int conn_fd, MSG *message);           //解散群

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
                int cfd = accept(lfd, (struct sockaddr*)&client_addr, &cli_len);
                //cfd上树
                ev.events = EPOLLIN;
                ev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                
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

                //printf("C\n");
                if(len == 0) {
                    //检测的fd从树上删除
                    char ip[36];
                    printf("client IP: %s\t\tport: %d 已断开连接\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)),
                            ntohs(client_addr.sin_port));
                    del_node(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }

                //写数据，处理客户端的请求
                chuli_request(fd, &message);
            }
        }
    }
    return 0;
}
//尾插法
//void charu_list(Node *phead, user_date date) {
/*void charu_list(user_date date){
    Node *current, *new_node;
    new_node = (Node*)malloc(sizeof(Node));
    
    //init要插入节点
    //strcpy(node->date.name, date.name);
    //node->date.socket = date.socket;
    new_node->date = date;
    new_node->next = NULL;
    
    if(head == NULL) {
        //printf("A\n");
        head = new_node;
        return;
    }
    
    //找到尾节点
    current = head;
    while(current->next != NULL) {
        current = current->next;
    }

    current->next = new_node;
    return;
}*/

void chuli_request(int conn_fd, MSG *message) {
    /*printf("-------------------\n");
    printf("from_name:\t%s\n", message->from_name);
    printf("to_name:\t%s\n", message->to_name);
    printf("-------------------\n");*/
    switch(message->cmd) {
    case 0:
        forget_password(conn_fd, message);
        break;
    case 1:
        reg(conn_fd, message);
        send(conn_fd, message, sizeof(MSG), 0);
        break;
    case 2:
        //printf("name:%s\n", message->user_infor.name);
        login(conn_fd, message);
        //printf("\n");
        //send(conn_fd, message, sizeof(MSG), 0);
        break;
    case 11:
        all_friends(conn_fd, message);
        break;
    case 12:
        online_friend(conn_fd, message);
        break;
    case 13:
        private_chat(conn_fd, message);
        break;
    case 14:
        add_friend(conn_fd, message);
        break;
    case 15:
        delete_friend(conn_fd, message);
        break;
    case 21:
        view_group(conn_fd, message);
        break;
    case 22:
        public_chat(conn_fd, message);
        break;
    case 23:
        //printf("A\n");
        add_group(conn_fd, message);
        break;
    case 24:
        quit_group(conn_fd, message);
        break;
    case 251:
        view_group_member(conn_fd, message);
        break;
    case 252:
        break_group(conn_fd, message);
        break;
    case 311:
        deal_friend_request_y(conn_fd, message);
        break;
    case 312:
        deal_friend_request_n(conn_fd, message);
        break;
    case 321:
        deal_group_request_y(conn_fd, message);
        break;
    case 322:
        deal_group_request_n(conn_fd, message);
        break;
    case 5:
        create_group(conn_fd, message);
        break;
    case 6:
        send_file(conn_fd, message);
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
    sprintf(str, "insert into user values(NULL, '%s','%s','%s','%s','%s')",
            message->user_infor.name, message->user_infor.nickname, message->user_infor.sex, message->user_infor.password, message->user_infor.telephone);
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
    //printf("type = %c\n", message->type);

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
        send(client_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }
    
    printf("%s 登陆成功\n", message->user_infor.name);
    message->cmd = 1002;
    strcpy(date.name, message->user_infor.name);
    //printf("date.name: %s\n", date.name);
    date.socket = client_fd;
    //printf("socket: %d\n", date.socket);
    charu_list(date);
    //list_num();
    send(client_fd, message, sizeof(MSG), 0);

    //查看该用户是否有离线请求
    //如果该用户有离线请求，发送离线请求
    request_node *current = head_request;
    while(current != NULL){
        if(strcmp(current->to_name, message->user_infor.name) == 0){
            switch(current->type){
            case 'f':  //好友申请
                strcpy(message->from_name, current->from_name);
                strcpy(message->to_name, current->to_name);
                message->cmd = 14;
                send(client_fd, message, sizeof(MSG), 0);
                break;
            case 'y':  //同意好友申请
                strcpy(message->from_name, current->from_name);
                strcpy(message->to_name, current->to_name);
                message->cmd = 311;
                send(client_fd, message, sizeof(MSG), 0);
                break;
            case 'n':  //拒绝好友申请
                strcpy(message->from_name, current->from_name);  //申请方
                strcpy(message->to_name, current->to_name);  //处理方
                message->cmd = 312;
                send(client_fd, message, sizeof(MSG), 0);
                break;
            case 'g':
                strcpy(message->from_name, current->from_name); //申请方
                strcpy(message->to_name, current->to_name);  //群主
                message->cmd = 23;
                send(client_fd, message, sizeof(MSG), 0);
                break;
            case 'o':  //同意入群申请
                strcpy(message->from_name, current->from_name);
                strcpy(message->to_group_name, current->to_name);
                strcpy(message->to_group_name, current->to_group_name);
                message->cmd = 321;
                send(client_fd, message, sizeof(MSG), 0);
                break;
            case 'x':  //拒绝入群申请
                strcpy(message->from_name, current->from_name);
                strcpy(message->to_name, current->to_name);
                strcpy(message->to_group_name, current->to_group_name);
                message->cmd = 322;
                send(client_fd, message, sizeof(MSG), 0);
                break;
            case 'q':  //群成员退出群聊
                strcpy(message->from_name, current->from_name);
                strcpy(message->to_name, current->to_name);
                strcpy(message->to_group_name, current->to_group_name);
                message->cmd = 24;
                send(client_fd, message, sizeof(MSG), 0);
                break;
            }
        }
        current = current->next;
    }
    
    //释放结果集
    mysql_free_result(mysql_res);
    //send(client_fd, &message, sizeof(message), 0);
    
    mysql_close(&mysql);
}

void forget_password(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    if(NULL == mysql_init(&mysql)){
        printf("line: %d ", __LINE__-1);
        printf("mysql_init: %s", mysql_error(&mysql));
        exit(1);
    }
    if(NULL == mysql_real_connect(&mysql, "localhost", "root", "hqw479.#", "chat_room", 0, NULL, 0)){
        printf("line: %d ", __LINE__-1);
        printf("mysql_real_connect(): %s", mysql_error(&mysql));
        exit(1);
    }
    //查找是否存在该账户
    char str[200];
    memset(str, 0, sizeof(str));
    sprintf(str, "select * from user where u_name = '%s'",
                message->user_infor.name);
    //printf("str:%s\n", str);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d", __LINE__-2);
        printf("mysql_real_query() error\n");
        mysql_close(&mysql);
        exit(1);
    }

    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    int rows = mysql_num_rows(mysql_res);
    if(rows == 0){
        message->cmd = 01;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    sprintf(str, "select * from user where u_name = '%s' and u_telephone = '%s'",
                message->user_infor.name, message->user_infor.telephone);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d", __LINE__-2);
        printf("mysql_real_query(): error\n");
        exit(1);
    }

    mysql_res = mysql_store_result(&mysql);
    if(!mysql_res){
        printf("line: %d", __LINE__-2);
        printf("mysql_store_result(): error\n");
        exit(1);
    }
    rows = mysql_num_rows(mysql_res);
    if(rows == 0){
        message->cmd = 02;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }
    
    message->cmd = 03;
    MYSQL_ROW row;
    row = mysql_fetch_row(mysql_res);
    strcpy(message->user_infor.password, row[4]);
    send(conn_fd, message, sizeof(MSG), 0);
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
    return;;
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
    //printf("%s", str);

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
        message->cmd = 111;
        mysql_free_result(mysql_res);
        send(client_fd, message, sizeof(MSG), 0);
        return;
    }
    while(row = mysql_fetch_row(mysql_res)){
        //strcpy(message->friend_name, row[1]);
        //printf("friend: %s\n", message->friend_name);
        //send(client_fd, message, sizeof(MSG), 0);
        if(strcmp(row[4], "1") == 0){
            strcpy(message->friend_name, row[1]);
            send(client_fd, message, sizeof(MSG), 0);
        }
    }
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}

void online_friend(int client_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    //connect_mysql(&mysql);
    if(NULL == mysql_init(&mysql)){
       printf("mysql_init(): %s\n", mysql_error(&mysql));
       exit(1);
    }
    if(NULL == mysql_real_connect(&mysql, "localhost", "root", "hqw479.#", "chat_room", 0, NULL, 0)){
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        exit(1);
    }
    
    char str[200];
    sprintf(str, "select * from friend where f_username = '%s'",
                message->user_infor.name);
    //printf("%s\n", str);

    //获得结果集
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d\n", __LINE__-2);
        printf("mysql_real_query() error\n");
        mysql_close(&mysql);
        exit(1);
    }

    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(!mysql_res){
        printf("line: %d\n", __LINE__-2);
        printf("mysql_store_result() error\n");
        mysql_close(&mysql);
        exit(1);
    }

    int rows;
    MYSQL_ROW row;
    rows = mysql_num_rows(mysql_res);
    
    if(rows == 0){   //没有好友
        message->friend_num = 0;
        message->cmd = 121;
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
            if(strcmp(current->date.name, row[1])==0&&strcmp("1", row[4])==0){
                flag = 1;
                break;
            }
            current = current->next;
        }
        if(flag == 1){
            strcpy(message->friend_name, row[1]);
            send(client_fd, message, sizeof(MSG), 0);    
        }
    }

    if(flag == 0){
        message->cmd = 122;
        send(client_fd, message, sizeof(MSG), 0);
    }
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}

void private_chat(int conn_fd, MSG *message){
    MYSQL mysql;
    connect_mysql(&mysql);
    
    //查看对方是不是发送者的好友
    char str[4096];
    sprintf(str, "select * from friend where f_friendname = '%s' and f_username = '%s'",
                message->to_name, message->from_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d ", __LINE__-2);
        printf("mysql_store_result(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    int rows = mysql_num_rows(mysql_res);
    if(rows == 0){   //对方不是发送者的好友
        message->cmd = 130;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //查看发送者是否是对方的好友
    sprintf(str, "select * from friend where f_friendname = '%s' and f_username = '%s'",
                message->from_name, message->to_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d ", __LINE__-2);
        printf("mysql_store_result(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    rows = mysql_num_rows(mysql_res);
    if(rows == 0){  //发送者不是对方的好友，可能被对方删除了
        message->cmd = 131;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }
    
    //聊天记录存入数据库
    //char str[4096];
    //sprintf(str, "insert into chat_record values(NULL,'%s','%s','%s','0','%s')",
    //        message->news, message->to_name, message->from_name, message->time);

    //查看是否在线
    Node *current = head;
    int flag = 0;
    while(current != NULL){
        if(strcmp(current->date.name, message->to_name) == 0){
            flag = 1;
            break;
        }
        current = current->next;
    }
    //未找到目标用户，目标用户不在线，储存在消息记录中
    if(flag == 0){
        //printf("未找到用户\n");
        //message->cmd = 131;
        //send(conn_fd, message, sizeof(MSG), 0);
        sprintf(str, "insert into chat_record values(NULL,'%s','%s','%s','0','%s')",
                message->news, message->to_name, message->from_name, message->time);
        mysql_real_query(&mysql, str, sizeof(str));
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //找到目标用户
    sprintf(str, "insert into chat_record values(NULL,'%s','%s','%s','1','%s')",
            message->news, message->to_name, message->from_name, message->time);
    mysql_real_query(&mysql, str, strlen(str));
    send(current->date.socket, message, sizeof(MSG), 0);
}

void add_friend(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);
    char str[200];
    //判断是否有该用户----------------------------------------------
    sprintf(str, "select * from user where u_name = '%s'", message->to_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        mysql_close(&mysql);
        exit(1);
    }
    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d error\n", __LINE__-2);
        mysql_close(&mysql);
        exit(1);
    }
    int rows = mysql_num_rows(mysql_res);
    if(!rows){
        message->cmd = 141;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //查看是否添加过好友
    sprintf(str, "select * from friend where f_friendname = '%s' and f_username = '%s'",
                message->to_name, message->from_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    //MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(!mysql_res){
        printf("line: %d ", __LINE__-2);
        printf("mysql_store_result(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    rows = mysql_num_rows(mysql_res);
    if(rows){  //已经是好友
        message->cmd = 140;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //如果未添加过的好友，则开始添加好友
    sprintf(str, "insert into friend values(NULL,'%s','%s','1','0')",
                message->to_name, message->from_name); 
    mysql_real_query(&mysql, str, strlen(str));
    //找到要加的好友节点
    Node *current = head;
    int flag = 0;

    while(current != NULL){
        if(strcmp(current->date.name, message->to_name) == 0){
            flag = 1;
            break;
        }
        current = current->next;
    }
    //要加的好友不在线，请求加入到请求链表中
    if(flag == 0){
        //message->cmd = -14;
        //send(conn_fd, message, sizeof(MSG), 0);
        request_node *current, *new_node;
        new_node = (request_node*)malloc(sizeof(request_node));
        //init
        strcpy(new_node->from_name, message->from_name);
        strcpy(new_node->to_name, message->to_name);
        new_node->type = 'f';
        new_node->next = NULL;

        if(head_request == NULL){
            head_request = new_node;
            return;
        }
        
        current = head_request;
        while(current->next != NULL){
            current = current->next;
        }
        current->next = new_node;
        return;
    }
    //message->cmd = 141;
    //找到目标用户
    send(current->date.socket, message, sizeof(MSG), 0);
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
    return;
}

void delete_friend(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);

    //查看是否有该好友
    char str[1024];
    memset(str, 0, sizeof(str));
    sprintf(str, "select * from friend where f_friendname = '%s' and f_username = '%s'",
                message->to_name, message->from_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }

    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d", __LINE__-2);
        printf("mysql_store_result(): error\n");
        mysql_close(&mysql);
        exit(1);
    }

    int rows = mysql_num_rows(mysql_res);
    if(rows == 0){  //没有该好友
        message->cmd = 150;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //如果有该好友，单方面删除该好友
    sprintf(str, "delete from friend where f_friendname = '%s' and f_username = '%s'",
                message->to_name, message->from_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }

    //通知删除好友成功
    send(conn_fd, message, sizeof(MSG), 0);
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
    return;
}

void view_group(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);
   
    //查看我加入的群
    char str[4096];
    sprintf(str, "select * from group_relationship where user_name = '%s'",
                message->user_infor.name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("mysql_real_query() error\n");
        mysql_close(&mysql);
        exit(1);
    }

    MYSQL_RES *mysql_res;
    mysql_res = mysql_store_result(&mysql);
    if(!mysql_res){
        printf("line: %d", __LINE__-2);
        printf("mysql_store_result() error\n");
        exit(1);
    }

    int rows;
    MYSQL_ROW row;
    rows = mysql_num_rows(mysql_res);
    if(rows == 0){  //没有群聊
        message->cmd = 211;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }
    //printf("cmd = %d\n", message->cmd);
    while(row = mysql_fetch_row(mysql_res)){
        strcpy(message->group.group_name, row[0]);
        //printf("message->group.group_name: %s\n", message->group.group_name);
        //message->cmd = 21;
        send(conn_fd, message, sizeof(MSG), 0);
    }
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}

void public_chat(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);

    //查看发送者是否有该群
    char str[4096];
    sprintf(str, "select * from group_relationship where group_name = '%s' and user_name = '%s'",
                message->to_group_name, message->from_name);

    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        mysql_close(&mysql);
        exit(1);
    }

    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d ", __LINE__-2);
        printf("mysql_store_result(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    int rows = mysql_num_rows(mysql_res);
    if(rows == 0){  //没有该群
        message->cmd = 220;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //发送者有该群
    //通过群名找到目标用户
    //char str[4096];
    sprintf(str, "select * from group_relationship where group_name = '%s'",
                message->to_group_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d\n", __LINE__-2);
        printf("mysql_real_query() error\n");
        exit(1);
    }

    //获得结果集
    //MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(!mysql_res){
        printf("line: %d\n", __LINE__-2);
        printf("mysql_store_result() error\n");
        exit(1);
    }

    //int rows;
    MYSQL_ROW row;
    //rows = mysql_num_rows(mysql_res);
    while(row = mysql_fetch_row(mysql_res)){
        printf("row[1]:%s \t from_name:%s\n", row[1], message->from_name);
        //如果是自己就跳过
        if(strcmp(row[1], message->from_name) == 0){
            continue;
        }
        //查看目标用户是否在线
        Node *current = head;
        int flag = 0;
        while(current != NULL){
            if(strcmp(current->date.name, row[1]) == 0){
                flag = 1;
                break;
            }
            current = current->next;
        }
        //将目标用户储存在message中
        strcpy(message->to_name, row[1]);
        //未找到目标用户，目标用户不在线，储存在群消息记录中
        if(flag == 0){
            sprintf(str, "insert into group_record values(NULL,'%s','%s','%s','%s','0','%s')",
                        message->news, message->to_name, message->from_name, message->to_group_name, message->time);
            mysql_real_query(&mysql, str, strlen(str));
            continue;
        }
        //找到目标用户，目标用户在线
        sprintf(str, "insert into group_record values(NULL,'%s','%s','%s','%s','1','%s')",
                    message->news, message->to_name, message->from_name, message->to_group_name, message->time);
        mysql_real_query(&mysql, str, strlen(str));
        send(current->date.socket, message, sizeof(MSG), 0);
    }
}

void add_group(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);

    //查看是否已经加入过该群
    char str[1024];
    sprintf(str, "select * from group_relationship where group_name = '%s' and user_name = '%s'",
                message->to_group_name, message->from_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }

    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d ", __LINE__-2);
        printf("mysql_store_result(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    int rows = mysql_num_rows(mysql_res);
    if(rows){   //已经加入该群
        message->cmd = 230;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //没有加入过该群
    //根据群名在数据库中找到群主
    char owner_name[200];  //储存群主名
    //char str[4096];

    //根据群名，查询群主
    sprintf(str, "select * from user_groups where name = '%s'",
                message->to_group_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d", __LINE__-2);
        printf("mysql_real_query() error\n");
        exit(1);
    }
    //MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(!mysql_res){
        printf("line: %d", __LINE__-2);
        printf("mysql_store_result() error\n");
        exit(1);
    }

    MYSQL_ROW row;
    row = mysql_fetch_row(mysql_res);
    strcpy(owner_name, row[1]);
    strcpy(message->to_name, owner_name);
    //printf("owner_name: %s\n", message->to_name);
    //查看群主是否在线，在线发送请求，不在线发送离线请求
    int flag = 0;
    Node *current = head;
    while(current != NULL){
        if(strcmp(current->date.name, message->to_name) == 0){
            flag = 1;
            break;
        }
        current = current->next;
    }
    
    if(flag == 0){
        //message->cmd = -14;
        //send(conn_fd, message, sizeof(MSG), 0);
        request_node *current, *new_node;
        new_node = (request_node*)malloc(sizeof(request_node));
        //init
        strcpy(new_node->from_name, message->from_name);
        strcpy(new_node->to_name, message->to_name);
        new_node->type = 'g';
        new_node->next = NULL;

        if(head_request == NULL){
            head_request = new_node;
            return;
        }

        current = head_request;
        while(current->next != NULL){
            current = current->next;
        }
        current->next = new_node;
        return;
    }

    send(current->date.socket, message, sizeof(MSG), 0);
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}

void quit_group(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);

    //查看是否存在该群
    char str[200];
    memset(str, 0, sizeof(str));
    sprintf(str, "select * from user_groups where name = '%s'",
                message->to_group_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }

    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    int rows = mysql_num_rows(mysql_res);
    if(!rows){  //该群不存在
        message->cmd = 240;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }
    //群存在，根据群名找到群主
    MYSQL_ROW row;
    row = mysql_fetch_row(mysql_res);
    if(strcmp(message->from_name, row[1]) == 0){  //要退出的是群主
        message->cmd = 241;  //不能退出
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    strcpy(message->to_name, row[1]);

    //在群关系表中删除记录
    sprintf(str, "delete from group_relationship where group_name = '%s' and user_name = '%s'",
                message->to_group_name, message->from_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }

    //通知群主该成员退出
    //查看群主是否在线
    int flag = 0;
    Node *current = head;
    while(current != NULL){
        if(strcmp(current->date.name, message->to_name)){
            flag = 1;
            break;
        }
        current = current->next;
    }

    if(flag == 0){  //群主不在线
        request_node *current, *new_node;
        new_node = (request_node*)malloc(sizeof(request_node));
        //init
        strcpy(new_node->from_name, message->from_name);
        strcpy(new_node->to_name, message->to_name);
        strcpy(new_node->to_group_name, message->to_group_name);
        new_node->type = 'q';
        new_node->next = NULL;

        if(head_request == NULL){
            head_request = new_node;
            return;
        }

        current = head_request;
        while(current->next != NULL){
            current = current->next;
        }
        current->next = new_node;
        return;
    }

    //群主在线
    send(current->date.socket, message, sizeof(MSG), 0);
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}

void view_group_member(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);

    char str[200];
    //查看是否存在该群
    sprintf(str, "select * from user_groups where name = '%s'",
                message->to_group_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line:%d error\n", __LINE__-2);
        mysql_close(&mysql);
        exit(1);
    }

    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d error\n", __LINE__-2);
        mysql_close(&mysql);
        exit(1);
    }

    int rows = mysql_num_rows(mysql_res);
    if(!rows){  //没有该群
        message->cmd = 2510;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //查看该人是否在群中
    sprintf(str, "select * from group_relationship where group_name = '%s', user_name = '%s'",
                message->to_group_name, message->from_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d error\n", __LINE__-2);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }
    rows = mysql_num_rows(mysql_res);
    if(!rows){ //没有加入该群
        message->cmd = 2511;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //查看群成员
    sprintf(str, "select * from group_relationship where group_name = '%s'",
                message->to_group_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d error\n", __LINE__-2);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }

    MYSQL_ROW row;
    while(row = mysql_fetch_row(mysql_res)){
        strcpy(message->to_name, row[1]);
        send(conn_fd, message, sizeof(MSG), 0);
    }

    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}

void break_group(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);

    //查看是否存在该群
    char str[200];
    sprintf(str, "select * from user_groups where name = '%s'",
                message->to_group_name);
    int ret = mysql_real_query(&mysql, str, sizeof(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        mysql_close(&mysql);
    }
    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d error\n", __LINE__-2);
        mysql_close(&mysql);
    }
    int rows = mysql_num_rows(mysql_res);
    if(!rows){
        message->cmd = 2520;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //查看操作者是否是群主
    MYSQL_ROW row;
    row = mysql_fetch_row(mysql_res);
    if(strcmp(message->from_name, row[1]) != 0){
        message->cmd = 2521;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //是群主，1.删除群，2.在群关系表中也删除该群
    sprintf(str, "delete from user_groups where name = %s", message->to_group_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }

    sprintf(str, "delete from group_relationship where group_name = '%s'",
                message->to_group_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }

    //通知解散完成
    send(conn_fd, message, sizeof(MSG), 0);
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}

void deal_friend_request_y(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);
    //将好友关系插入到好友表，处理状态为1
    char str[200];
    sprintf(str, "insert into friend values(NULL,'%s','%s','1','1')",
                message->from_name, message->to_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    //将申请方的处理状态置为1;
    sprintf(str, "update friend set status = 1 where f_friendname = '%s' and f_username = '%s',",
                    message->to_name, message->from_name);
    mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }

    //找到要通知的用户节点
    int flag = 0;
    Node *current = head;
    while(current != NULL){
        if(strcmp(current->date.name, message->from_name) == 0){
            flag = 1;
            break;
        }
        current = current->next;
    }
    //要通知的用户不在线
    if(flag == 0){
        request_node *current, *new_node;
        new_node = (request_node*)malloc(sizeof(request_node));
        //init
        strcpy(new_node->from_name, message->from_name);
        strcpy(new_node->to_name, message->to_name);
        new_node->type = 'y';
        new_node->next = NULL;

        if(head_request == NULL){
            head_request = new_node;
            return;
        }

        current = head_request;
        while(current->next != NULL){
            current = current->next;
        }
        current->next = new_node;
        return;
    }

    send(current->date.socket, message, sizeof(MSG), 0);
    //关闭数据库
    mysql_close(&mysql);
}

void deal_friend_request_n(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);

    //在好友表中将申请方的记录删除
    char str[200];
    memset(str, 0, sizeof(str));
    sprintf(str, "delete from friend where f_friendname = %s and f_username = %s",
                message->to_name, message->from_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }

    //通知申请方
    //找到要通知的节点
    int flag = 0;
    Node *current = head;
    while(current != NULL){
        if(strcmp(current->date.name, message->from_name) == 0){
            flag = 1;
            break;
        }
        current = current->next;
    }
    
    //申请方不在线
    if(flag == 0){
        request_node *current, *new_node;
        new_node = (request_node*)malloc(sizeof(request_node));
        //init
        strcpy(new_node->from_name, message->from_name);
        strcpy(new_node->to_name, message->to_name);
        new_node->type = 'n';
        new_node->next = NULL;

        if(head_request == NULL){
            head_request = new_node;
            return;
        }

        current = head_request;
        while(current->next != NULL){
            current = current->next;
        }
        current->next = new_node;
        return;
    }
    
    //申请方在线
    send(current->date.socket, message, sizeof(MSG), 0);
    mysql_close(&mysql);
}

void deal_group_request_y(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);

    //根据群主名找到群名
    char str[200];
    /*sprintf(str, "select * from user_groups where owner = '%s'", message->to_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d", __LINE__-2);
        printf("mysql_real_query() error\n");
        exit(1);
    }
    //获得结果集
    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(!mysql_res){
        printf("line: %d", __LINE__-2);
        printf("mysql_store_result() error\n");
        exit(1);
    }
    MYSQL_ROW row;
    row = mysql_fetch_row(mysql_res);
    char group_name[20];
    strcpy(group_name, row[0]);
    */
    //printf("group_name: %s\n", group_name);
    //插入群关系表
    sprintf(str, "insert into group_relationship values('%s','%s','3')",
                message->to_group_name, message->from_name);
    //printf("str: %s\n", str);
    mysql_real_query(&mysql, str, strlen(str));

    //找到通知的节点
    int flag = 0;
    Node *current = head;
    while(current != NULL){
        if(strcmp(current->date.name, message->from_name) == 0){
            flag = 1;
            break;
        }
        current = current->next;
    }
    if(flag == 0){ 
        request_node *current, *new_node;
        new_node = (request_node*)malloc(sizeof(request_node));
        //init
        strcpy(new_node->from_name, message->from_name);
        strcpy(new_node->to_name, message->to_group_name);
        strcpy(new_node->to_group_name, message->to_group_name);
        new_node->type = 'o';
        new_node->next = NULL;

        if(head_request == NULL){
            head_request = new_node;
            return;
        }

        current = head_request;
        while(current->next != NULL){
            current = current->next;
        }
        current->next = new_node;
        return;
    }

    //strcpy(message->to_name, group_name);

    send(current->date.socket, message, sizeof(MSG), 0);
    //mysql_free_result(mysql_res);
    mysql_close(&mysql);
}

void deal_group_request_n(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);
    
    //找到要通知的节点
    int flag = 0;
    Node *current = head;
    while(current != NULL){
        if(strcmp(current->date.name, message->from_name)){
            flag = 1;
            break;
        }
        current = current->next;
    }

    if(flag == 0){
        request_node *current, *new_node;
        new_node = (request_node*)malloc(sizeof(request_node));
        //init
        strcpy(new_node->from_name, message->from_name);
        strcpy(new_node->to_name, message->to_group_name);
        strcpy(new_node->to_group_name, message->to_group_name);
        new_node->type = 'x';
        new_node->next = NULL;

        if(head_request == NULL){
            head_request = new_node;
            return;
        }

        current = head_request;
        while(current->next != NULL){
            current = current->next;
        }
        current->next = new_node;
        return;
    }

    send(current->date.socket, message, sizeof(MSG), 0);
}

void create_group(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);

    //查看该群是否存在
    char str[4096];
    sprintf(str, "select * from user_groups where name = '%s'",
                message->group.group_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    int rows = mysql_num_rows(mysql_res);
    if(rows){  //该群已存在
        message->cmd = 50;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    //如果不存在，创建群
    sprintf(str, "insert into user_groups values('%s','%s')",
                message->group.group_name, message->group.group_owner);
    //printf("str: %s\n", str);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }
    //将群与群主加入群关系表中
    sprintf(str, "insert into group_relationship values('%s','%s','1')",
                message->group.group_name, message->group.group_owner);
    //printf("message->cmd = %d\n", message->cmd);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }
    
    send(conn_fd, message, sizeof(MSG), 0);
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}

char *get_file_name(char *file_path){
    int len = strlen(file_path);
    int j;
    char *file_name;
    file_name = (char*)malloc(sizeof(char)*256);
    
    for(j = len-1; file_path[j-1] != '/'; j--); //找到最后一个 / 前一个字母

    //记录文件名
    int i = 0;
    for(j; j<len; j++){
        file_name[i] = file_path[j];
        i++;
    }
    file_name[i] = '\0';

    return file_name;
}

void send_file(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);
    
    //查看是否有目标用户
    char str[200];
    sprintf(str, "select * from user where u_name = '%s'", message->to_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    MYSQL_RES *mysql_res = NULL;
    mysql_res = mysql_store_result(&mysql);
    if(mysql_res == NULL){
        printf("line: %d ", __LINE__-2);
        printf("mysql_store_result(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    int rows = mysql_num_rows(mysql_res);
    if(!rows){
        message->cmd = 60;
        send(conn_fd, message, sizeof(MSG), 0);
        mysql_free_result(mysql_res);
        mysql_close(&mysql);
        return;
    }

    int fd;
    char buf[256];  //临时储存文件的路径
    
    //获得文件名
    char file_name[256];
    strcpy(file_name, get_file_name(message->file_path));
    strcpy(message->file_name, file_name);
    
    //临时存储
    sprintf(buf, "/home/huloves/chat_room_file/%s", file_name);
    //printf("buf:%s\n", buf);
    //strcpy(message->file_name, file_name);
    fd = open(buf, O_RDWR | O_CREAT, 0644);
    if(fd == -1){
        my_err("open", __LINE__-2);
    }

    //int size = 0;

    //接收文件
    //printf("size = %d\n", message->once_len);
    while(recv(conn_fd, message, sizeof(MSG), 0)){
        //printf("A\n");
        if(message->send_end == 1){
            printf("接收完成...\n");
            break;
        }
        printf("正在接收文件......size = %d\n", message->once_len);
        write(fd, message->news, message->once_len);
    }

    //查看目标用户是否在线
    int flag = 0;
    Node *current = head;
    while(current != NULL){
        //printf("A\n");
        if(strcmp(current->date.name, message->to_name) == 0){
            flag = 1;
            break;
        }
        current = current->next;
    }

    if(flag == 0){  //不在线
        printf("目标不在线\n");
        return;
    }

    //通知目标用户有文件要接受
    strcpy(message->file_name, file_name);
    printf("*********file_name:%s\n", message->file_name);
    send(current->date.socket, message, sizeof(MSG), 0);

    if(lseek(fd, 0, SEEK_SET) == -1){
        my_err("lseek", __LINE__-1);
    }

    //发送文件
    int size = 0;
    message->send_end = 0;
    while(size = read(fd, message->news, sizeof(message->news))){
        printf("正在发送文件... size = %d\n", size);
        message->once_len = size;
        send(current->date.socket, message, sizeof(MSG), 0);
    }
    printf("发送结束...\n");
    message->send_end = 1;
    send(current->date.socket, message, sizeof(MSG), 0); //通知发送完成
    
    close(fd);
    mysql_free_result(mysql_res);
    mysql_close(&mysql);
}
