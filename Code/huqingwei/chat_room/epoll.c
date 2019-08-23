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
#include <signal.h>

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

/*typedef struct Node_mysql_user{  //用户表链表
    mysql_user user;
    struct Node_mysql_user *next;
}Node_mysql_user;

typedef struct Node_mysql_friend{  //好友表链表
    mysql_friend friends;
    struct Node_mysql_friend *next;
}Node_mysql_friend;

typedef struct Node_mysql_user_groups{ //群组链表
    mysql_user_groups user_groups;
    Node_mysql_user_groups *next;
}Node_mysql_user_groups;

typedef struct Node_mysql_group_relationship{  //群成员关系链表
    mysql_group_relationship group_relationship;
    struct Node_mysql_group_relationship *next;
}Node_mysql_group_relationship;*/

typedef struct Item{
    mysql_user user;
    mysql_friend friends;
    mysql_user_groups user_groups;
    mysql_group_relationship group_relationship;
}Item;

typedef struct mysql_node{
    Item item;
    struct mysql_node *next;
}mysql_node;

mysql_node *head_user = NULL; //用户链表
mysql_node *head_friends = NULL; //好友链表
mysql_node *head_user_groups = NULL; //群组链表
mysql_node *head_group_relationship = NULL; //群关系链表

Node *head = NULL;  //创建头指针
request_node *head_request = NULL;  //创建请求队列头指针

MYSQL mysql;

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
void charu_list(user_date date){
    Node *current, *new_node;
    new_node = (Node*)malloc(sizeof(Node));
    
    //init要插入节点
    new_node->date = date;
    new_node->next = NULL;
    
    if(head == NULL) {
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
void online_user_list_num(){
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

//删除在线用户节点
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

//尾插法——数据库相关链表
void weicha_list(mysql_node **phead, mysql_node *new_node){
    mysql_node *current;  
    
    if(*phead == NULL){
        *phead = new_node;
        return;
    }

    current = *phead;
    while(current->next != NULL){
        current = current->next;
    }

    current->next = new_node;
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
void set_manager(int conn_fd, MSG *message);             //设置管理员

void read_mysql_user(mysql_node **phead);  //读取用户表
void read_mysql_friend(mysql_node **phead); //读取好友表
void read_mysql_user_groups(mysql_node **phead);  //读取群组表
void read_mysql_group_relationship(mysql_node **phead); //读取群关系表

void free_list(mysql_node **phead); //释放资源

//信号处理函数
int server_close;
void headler(int signa){
    server_close = 1;
}

int main()
{
    if(signal(SIGINT, headler) == SIG_ERR){
        my_err("signal", __LINE__-1);
    }

    //连接数据库
    connect_mysql(&mysql);

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

    //读取数据库信息
    read_mysql_user(&head_user);
    read_mysql_friend(&head_friends);
    read_mysql_user_groups(&head_user_groups);
    read_mysql_group_relationship(&head_group_relationship);

    printf("等待连接......\n");

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
                if(!(all[i].events & EPOLLIN)) {
                    continue;
                }

                //读数据
                MSG message;
                int len = recv(fd, &message, sizeof(MSG), 0);
                printf("------cmd: %d\n", message.cmd);
                //printf("has been received: %d :message:%ld\n",sum,sizeof(message));
                if(len == 0) {
                    //检测的fd从树上删除
                    char ip[36];
                    printf("client IP: %s\t\tport: %d 已断开连接\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)),
                            ntohs(client_addr.sin_port));
                    del_node(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }

                printf("AAAAAAAAAAAAA\n");
                //写数据，处理客户端的请求
                chuli_request(fd, &message);
            }
        }
        if(server_close == 1){
            break;
        }
    }
    
    //释放资源
    free_list(&head_user);
    free_list(&head_friends);
    free_list(&head_user_groups);
    free_list(&head_group_relationship);
    close(epfd);
    
    printf("服务器已关闭\n");
    return 0;
}

void chuli_request(int conn_fd, MSG *message) {
    switch(message->cmd) {
    case 0:
        forget_password(conn_fd, message);
        break;
    case 1:
        reg(conn_fd, message);
        break;
    case 2:
        login(conn_fd, message);
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
    case 253:
        set_manager(conn_fd, message);
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

void read_mysql_user(mysql_node **phead){
    MYSQL_RES *mysql_res = NULL;
    MYSQL_ROW row;
    char str[1024];

    mysql_node *current, *new_node;

    sprintf(str, "select * from user");
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
    }

    mysql_res = mysql_store_result(&mysql);
    
    while(row = mysql_fetch_row(mysql_res)){
        new_node = (mysql_node*)malloc(sizeof(mysql_node));
        strcpy(new_node->item.user.u_name, row[1]);
        strcpy(new_node->item.user.u_nickname, row[2]);
        strcpy(new_node->item.user.u_sex, row[3]);
        strcpy(new_node->item.user.u_passworld, row[4]);
        strcpy(new_node->item.user.telephone, row[5]);
        new_node->next = NULL;
        if(*phead == NULL){
            *phead = new_node;
        }
        else{
            current->next = new_node;
        }
        current = new_node;
    }
}

void read_mysql_friend(mysql_node **phead){
    MYSQL_RES *mysql_res = NULL;
    MYSQL_ROW row;
    char str[1024];

    mysql_node *current, *new_node;

    sprintf(str, "select * from friend");
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
    }
    
    mysql_res = mysql_store_result(&mysql);
    
    while(row = mysql_fetch_row(mysql_res)){
        new_node = (mysql_node*)malloc(sizeof(mysql_node));
        strcpy(new_node->item.friends.f_friendname, row[1]);
        strcpy(new_node->item.friends.f_username, row[2]);
        //strcpy(new_node->item.friends.type, row[3]);
        new_node->item.friends.type = atol(row[3]);
        //strcpy(new_node->item.friends.status, row[4]);
        new_node->item.friends.status = atol(row[4]);
        new_node->next = NULL;
        if(*phead == NULL){
            *phead = new_node;
        }
        else{
            current->next = new_node;
        }
        current = new_node;
    }
}

void read_mysql_user_groups(mysql_node **phead){
    MYSQL_RES *mysql_res = NULL;
    MYSQL_ROW row;
    char str[1024];

    mysql_node *current, *new_node;

    sprintf(str, "select * from user_groups");

    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
    }

    mysql_res = mysql_store_result(&mysql);

    while(row = mysql_fetch_row(mysql_res)){
        new_node = (mysql_node*)malloc(sizeof(mysql_node));
        strcpy(new_node->item.user_groups.name, row[0]);
        strcpy(new_node->item.user_groups.owner, row[1]);
        new_node->next = NULL;
        if(*phead == NULL){
            *phead = new_node;
        }
        else{
            current->next = new_node;
        }
        current = new_node;
    }
}

void read_mysql_group_relationship(mysql_node **phead){
    MYSQL_RES *mysql_res = NULL;
    MYSQL_ROW row;
    char str[1024];
    
    mysql_node *current, *new_node;

    sprintf(str, "select * from group_relationship");

    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
    }

    mysql_res = mysql_store_result(&mysql);

    while(row = mysql_fetch_row(mysql_res)){
        new_node = (mysql_node*)malloc(sizeof(mysql_node));
        strcpy(new_node->item.group_relationship.group_name, row[0]);
        strcpy(new_node->item.group_relationship.user_name, row[1]);
        new_node->item.group_relationship.type = atol(row[2]);
        new_node->next = NULL;
        if(*phead == NULL){
            *phead = new_node;
        }
        else{
            current->next = new_node;
        }
        current = new_node;
    }
}

void free_list(mysql_node **phead){
    mysql_node *temp = *phead;
    if(phead == NULL){
        return;
    }
    while(*phead){
        temp = (*phead)->next;
        free((*phead));
        *phead = temp;
    }
    phead = NULL;
}

void reg(int client_fd, MSG *message) {
    mysql_node *user, *current;
    user = (mysql_node*)malloc(sizeof(mysql_node));
    strcpy(user->item.user.u_name, message->user_infor.name);
    strcpy(user->item.user.u_nickname, message->user_infor.nickname);
    strcpy(user->item.user.u_sex, message->user_infor.sex);
    strcpy(user->item.user.u_passworld, message->user_infor.password);
    strcpy(user->item.user.telephone, message->user_infor.telephone);
    user->next = NULL;

    current = head_user;
    while(current != NULL){
        printf("u_name:%s\n", current->item.user.u_name);
        current = current->next;
    }

    int flag = 0;
    current = head_user;
    while(current != NULL){
        if(strcmp(current->item.user.u_name, user->item.user.u_name) == 0){
            flag = 1;
            break;
        }
        current = current->next;
    }

    if(flag == 1){
        printf("%s 注册失败，用户名重复\n", user->item.user.u_name);
        free(user);
        message->cmd = -1;
        send(client_fd, message, sizeof(MSG), 0);
        return;
    }
    
    char str[1024];

    weicha_list(&head_user, user);

    current = head_user;
    while(current != NULL){
        printf("----------name:%s\n", current->item.user.u_name);
        current = current->next;
    }
    
    sprintf(str, "insert into user values(NULL, '%s','%s','%s','%s','%s')",
                user->item.user.u_name, user->item.user.u_nickname, user->item.user.u_sex, user->item.user.u_passworld, user->item.user.telephone);
    printf("str:%s\n", str);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        exit(1);
    }
    printf("%s 注册成功\n", user->item.user.u_name);
    message->cmd = 1001;
    send(client_fd, message, sizeof(MSG), 0);
}

void login(int client_fd, MSG *message) {
    user_date date;    
    mysql_node *current_node;

    //test
    Node *current_Node = head;
    while(current_Node != NULL){
        printf("name: %s\n", current_Node->date.name);
        current_Node = current_Node->next;
    }
    
    //查看用户名，密码是否正确
    int flag = 0;
    current_node = head_user;
    while(current_node != NULL){
        if(strcmp(current_node->item.user.u_name, message->user_infor.name)==0&&
           strcmp(current_node->item.user.u_passworld, message->user_infor.password)==0){
            flag = 1;
            break;
        }
        current_node = current_node->next;
    }

    if(flag == 0){
        printf("%s 登录失败\n", message->user_infor.name);
        message->cmd = -2;
        send(client_fd, message, sizeof(MSG), 0);
        return;
    }
    
    message->cmd = 1002;
    strcpy(date.name, message->user_infor.name);
    //printf("date.name: %s\n", date.name);
    
    date.socket = client_fd;
    //printf("socket: %d\n", date.socket);

    charu_list(date);
    //list_num();
    current_Node = head;
    while(current_Node != NULL){
        printf("-------name: %s\n", current_Node->date.name);
        current_Node = current_Node->next;
    }
    
    printf("%s 登陆成功\n", message->user_infor.name);
    
    int len = sizeof(MSG);
    int size;
    if((size = send(client_fd, message, sizeof(MSG), 0)) != len){
        printf("send: %d\n", size);
    }
    printf("----send: %d\n", size);

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
}

void forget_password(int conn_fd, MSG *message){
    //查看是否存在该账户
    mysql_node *current;
    int flag = 0;
    current = head_user;
    while(current != NULL){
        if(strcmp(current->item.user.u_name, message->user_infor.name) == 0){
            flag = 1;
            break;
        }
        current = current->next;
    }
    if(flag == 0){
        message->cmd = 01;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //查看电话号码是否正确
    flag = 0;
    current = head_user;
    while(current != NULL){
        if(strcmp(current->item.user.u_name, message->user_infor.name)==0&&
            strcmp(current->item.user.telephone, message->user_infor.telephone)==0){
            flag = 1;
            break;
        }
        current = current->next;
    }
    if(flag == 0){
        message->cmd = 02;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }
    
    message->cmd = 03;
    strcpy(message->user_infor.password, current->item.user.telephone);
    send(conn_fd, message, sizeof(MSG), 0);
    return;;
}

void all_friends(int client_fd, MSG *message){
    //查看是否有好友
    mysql_node *current = NULL;
    int flag = 0;
    current = head_friends;
    while(current != NULL){
        if(strcmp(current->item.friends.f_username, message->user_infor.name)==0 && current->item.friends.status == 1){
            flag = 1;
            break;
        }
        current = current->next;
    }

    //如果没有好友
    if(flag == 0){
        message->cmd = 111;
        send(client_fd, message, sizeof(MSG), 0);
        return;
    }

    //如果有好友,发送好友
    current = head_friends;
    while(current != NULL){
        if(strcmp(current->item.friends.f_username, message->user_infor.name)==0 && current->item.friends.status == 1){
            strcpy(message->friend_name, current->item.friends.f_friendname);
            send(client_fd, message, sizeof(MSG), 0);
        }
        current = current->next;
    }
}

void online_friend(int client_fd, MSG *message){
    //查看有没有好友
    int flag = 0;
    //mysql_node *head_online_friend = NULL;
    mysql_node *current_friend = NULL;
    current_friend = head_friends;
    while(current_friend != NULL){
        if(strcmp(current_friend->item.friends.f_username, message->user_infor.name) == 0 && current_friend->item.friends.status == 1){
            flag = 1;
            break;
        }
        current_friend = current_friend->next;
    }
    if(flag == 0){  //没有好友
        message->cmd = 121;
        send(client_fd, message, sizeof(MSG), 0);
        return;
    }

    //临时储存在线好友
    mysql_node *current, *new_node, *head_online_friend = NULL, *end_online;
    current = head_friends;
    while(current != NULL){
        if(strcmp(current->item.friends.f_username, message->user_infor.name)==0 && current->item.friends.status == 1){
            new_node = (mysql_node*)malloc(sizeof(mysql_node));
            new_node->item = current->item;
            new_node->next = NULL;
            if(head_online_friend == NULL){
                head_online_friend = new_node;
            }
            else{
                end_online->next = new_node;
            }
            end_online = new_node;
        }
        current = current->next;
    }

    //查找在线好友
    current_friend = head_online_friend;
    while(current_friend != NULL){
        flag = 0;
        if(strcmp(current_friend->item.friends.f_username, message->user_infor.name)==0&&current_friend->item.friends.status == 1){
            Node *current_user = head;
            while(current_user != NULL){
                if(strcmp(current_user->date.name, current_friend->item.friends.f_friendname)==0){
                    flag = 1;
                    break;
                }
                current_user = current_user->next;
            }
        }
        if(flag == 1){
            strcpy(message->friend_name, current_friend->item.friends.f_friendname);
            send(client_fd, message, sizeof(MSG), 0);
        }
        current_friend = current_friend->next;
    }

    if(flag == 0){
        message->cmd = 122;
        send(client_fd, message, sizeof(MSG), 0);
    }
}

void private_chat(int conn_fd, MSG *message){
    MYSQL mysql;
    connect_mysql(&mysql);
    
    //查看对方是不是发送者的好友
    mysql_node *current = NULL;
    int flag = 0;
    current = head_friends;
    while(current != NULL){
        if(strcmp(current->item.friends.f_friendname, message->to_name)==0 && strcmp(current->item.friends.f_username, message->from_name)==0){
            flag = 1;
            break;
        }
        current = current->next;
    }
    if(flag == 0){
        message->cmd = 130;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //查看发送者是否是对方的好友
    current = head_friends;
    flag = 0;
    while(current != NULL){
        if(strcmp(current->item.friends.f_friendname, message->from_name)==0 && strcmp(current->item.friends.f_username, message->to_name)==0){
            flag = 1;
            break;
        }
        current = current->next;
    }
    if(flag == 0){
        message->cmd = 131;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }
    
    //聊天记录存入数据库

    //查看是否在线
    Node *current_online = head;
    int fflag = 0;
    while(current_online != NULL){
        if(strcmp(current_online->date.name, message->to_name) == 0){
            fflag = 1;
            break;
        }
        current_online = current_online->next;
    }

    char str[4096];
    //未找到目标用户，目标用户不在线，储存在消息记录中
    if(fflag == 0){
        printf("AAA\n");
        sprintf(str, "insert into chat_record values(NULL,'%s','%s','%s',0,'%s')",
                message->news, message->to_name, message->from_name, message->time);
        printf("str:%s\n", str);
        int ret = mysql_real_query(&mysql, str, strlen(str));
        if(ret){
            printf("line: %d error\n", __LINE__-2);
            exit(1);
        }
        return;
    }

    //找到目标用户
    sprintf(str, "insert into chat_record values(NULL,'%s','%s','%s',1,'%s')",
            message->news, message->to_name, message->from_name, message->time);
    printf("---str:%s\n", str);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        exit(1);
    }
    send(current_online->date.socket, message, sizeof(MSG), 0);
}

void add_friend(int conn_fd, MSG *message){
    printf("%s 正在加 %s 为好友\n", message->from_name, message->to_name);
    
    //不能和自己加好友
    if(strcmp(message->to_name, message->from_name) == 0){
        message->cmd = 149;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //判断是否有该用户
    mysql_node *current_user = NULL;
    int flag = 0;
    current_user = head_user;
    while(current_user != NULL){
        if(strcmp(current_user->item.user.u_name, message->to_name) == 0){
            flag = 1;
            break;
        }
        current_user = current_user->next;
    }
    if(flag == 0){
        message->cmd = 141;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //查看是否添加过好友
    mysql_node *current_friend = NULL;
    flag = 0;
    current_friend = head_friends;
    while(current_friend != NULL){
        if(strcmp(current_friend->item.friends.f_friendname, message->to_name)==0 && strcmp(current_friend->item.friends.f_username, message->from_name)==0){
            flag = 1;
            break;
        }
        current_friend = current_friend->next;
    }
    if(flag == 1){
        if(current_friend->item.friends.status == 1){  //已经是好友
            message->cmd = 140;
            send(conn_fd, message, sizeof(MSG), 0);
            return;
        }
        else if(current_friend->item.friends.status == 0){
            Node *current = head;
            while(current != NULL){
                if(strcmp(current->date.name, message->to_name) == 0){
                    break;
                }
                current = current->next;
            }
            send(current->date.socket, message, sizeof(MSG), 0);
            return;
        }
    }

    char str[1024];
    //如果未添加过的好友，则开始添加好友
    sprintf(str, "insert into friend values(NULL,'%s','%s', 1, 0)",
                message->to_name, message->from_name); 
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        exit(1);
    }
    //加入链表
    mysql_node *new_friend = NULL;
    new_friend = (mysql_node*)malloc(sizeof(mysql_node));
    strcpy(new_friend->item.friends.f_friendname, message->to_name);
    strcpy(new_friend->item.friends.f_username, message->from_name);
    new_friend->item.friends.type = 1;
    new_friend->item.friends.status = 0;
    new_friend->next = NULL;
    weicha_list(&head_friends, new_friend);

    //找到要加的好友节点
    Node *current = head;
    flag = 0;
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
    
    //找到目标用户
    send(current->date.socket, message, sizeof(MSG), 0);
    printf("已发送数据给 %s\n", current->date.name);
    return;
}

void delete_friend(int conn_fd, MSG *message){
    printf("%s 删除 %s\n", message->from_name, message->to_name);
    //查看是否有该好友
    char str[1024];
    mysql_node *current_friend = NULL;
    int flag = 0;
    current_friend = head_friends;
    while(current_friend != NULL){
        if(strcmp(current_friend->item.friends.f_friendname, message->to_name)==0&&
           strcmp(current_friend->item.friends.f_username, message->from_name)==0){
            flag = 1;
            break;
        }
        current_friend = current_friend->next;
    }
    if(flag == 0){
        message->cmd = 150;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //printf("current_friend->item.friends.status: %d\n", current_friend->item.friends.status);
    //查看对方有没有删除过自己
    if(current_friend->item.friends.status == 0){
        sprintf(str, "delete from friend where f_friendname = '%s' and f_username = '%s'",
                    message->to_name, message->from_name);
        int ret = mysql_real_query(&mysql, str, strlen(str));
        if(ret){
            printf("line: %d error\n", __LINE__-2);
            exit(1);
        }
        //在好友链表中删除
        mysql_node *delete_friend = head_friends;
        mysql_node *temp;
        while(delete_friend != NULL){
            if(strcmp(delete_friend->item.friends.f_friendname, message->to_name)==0&&strcmp(delete_friend->item.friends.f_username, message->from_name)==0){
                if(head_friends == delete_friend){
                    head_friends = delete_friend->next;
                    free(delete_friend);
                    break;
                }
                temp->next = delete_friend->next;
                break;
            }
            temp = delete_friend;
            delete_friend = delete_friend->next;
        }
        //通知删除好友成功
        send(conn_fd, message, sizeof(MSG), 0);
        return; 
    }

    //如果有该好友，单方面删除该好友
    sprintf(str, "delete from friend where f_friendname = '%s' and f_username = '%s'",
                message->to_name, message->from_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        //mysql_free_result(mysql_res);
        //mysql_close(&mysql);
        exit(1);
    }
    //在好友链表中删除
    mysql_node *delete_friend = head_friends;
    mysql_node *temp;
    while(delete_friend != NULL){
        if(strcmp(delete_friend->item.friends.f_friendname, message->to_name)==0&&strcmp(delete_friend->item.friends.f_username, message->from_name)==0){
            if(head_friends == delete_friend){
                head_friends = delete_friend->next;
                free(delete_friend);
                break;
            }
            temp->next = delete_friend->next;
                break;
        }
        temp = delete_friend;
        delete_friend = delete_friend->next;
    }


    //将对方的状态置为 0
    sprintf(str, "update friend set status = 0 where f_friendname = '%s' and f_username = '%s'",
                message->from_name, message->to_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        exit(1);
    }
    //好友链表中相应操作
    current_friend = head_friends;
    while(current_friend != NULL){
        if(strcmp(current_friend->item.friends.f_friendname, message->from_name) == 0 && strcmp(current_friend->item.friends.f_username, message->to_name) == 0){
            break;
        }
        current_friend = current_friend->next;
    }
    current_friend->item.friends.status = 0;

    //通知删除好友成功
    send(conn_fd, message, sizeof(MSG), 0);
    return;
}

void view_group(int conn_fd, MSG *message){
    mysql_node *current_group_relationship = NULL;
    int flag = 0;

    current_group_relationship = head_group_relationship;
    while(current_group_relationship != NULL){
        if(strcmp(current_group_relationship->item.group_relationship.user_name, message->user_infor.name)==0){
            flag = 1;
            strcpy(message->group.group_name, current_group_relationship->item.group_relationship.group_name);
            send(conn_fd, message, sizeof(MSG), 0);
        }
        current_group_relationship = current_group_relationship->next;
    }

    if(flag == 0){  //没有群聊
        message->cmd = 211;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }
}

void public_chat(int conn_fd, MSG *message){
    //连接数据库
    MYSQL mysql;
    connect_mysql(&mysql);

    //查看发送者是否有该群
    mysql_node *current_group_relationship = NULL;
    int fflag = 0;
    current_group_relationship = head_group_relationship;
    while(current_group_relationship != NULL){
        if(strcmp(current_group_relationship->item.group_relationship.group_name, message->to_group_name) == 0&&
           strcmp(current_group_relationship->item.group_relationship.user_name, message->from_name) == 0){
            fflag = 1;
            break;
        }
        current_group_relationship = current_group_relationship->next;
    }
    if(fflag == 0){
        message->cmd = 220;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //发送者有该群
    //通过群名找到目标用户
    char str[4096];
    current_group_relationship = head_group_relationship;
    while(current_group_relationship != NULL){
        if(strcmp(current_group_relationship->item.group_relationship.group_name, message->to_group_name) == 0){
            printf("row[1]:%s \t from_name:%s\n", current_group_relationship->item.group_relationship.user_name, message->from_name);
            //如果是自己就跳过
            if(strcmp(current_group_relationship->item.group_relationship.user_name, message->from_name) == 0){
                current_group_relationship = current_group_relationship->next;
                continue;
            }
            //查看目标用户是否在线
            Node *current = head;
            int flag = 0;
            while(current != NULL){
                if(strcmp(current->date.name, current_group_relationship->item.group_relationship.user_name) == 0){
                    flag = 1;
                    break;
                }
                current = current->next;
            }
            //将目标用户储存在message中
            strcpy(message->to_name, current_group_relationship->item.group_relationship.user_name);
            //未找到目标用户，目标用户不在线，储存在群消息记录中
            if(flag == 0){
                sprintf(str, "insert into group_record values(NULL,'%s','%s','%s','%s',0,'%s')",
                            message->news, message->to_name, message->from_name, message->to_group_name, message->time);
                printf("str:%s\n", str);
                int ret = mysql_real_query(&mysql, str, strlen(str));
                if(ret){
                    printf("line: %d error\n", __LINE__-2);
                    exit(1);
                }
                current_group_relationship = current_group_relationship->next;
                continue;
            }
            //找到目标用户，目标用户在线
            sprintf(str, "insert into group_record values(NULL,'%s','%s','%s','%s',1,'%s')",
                        message->news, message->to_name, message->from_name, message->to_group_name, message->time);
            printf("str:%s\n", str);
            int ret = mysql_real_query(&mysql, str, strlen(str));
            if(ret){
                printf("line: %d error\n", __LINE__-2);
                exit(1);
            }
            send(current->date.socket, message, sizeof(MSG), 0);
        }
        current_group_relationship = current_group_relationship->next;
    }
}

void add_group(int conn_fd, MSG *message){
    //查看是否已经加入过该群
    char str[1024];
    mysql_node *current_group_relationship = NULL;
    int fflag = 0;
    current_group_relationship = head_group_relationship;
    while(current_group_relationship != NULL){
        if(strcmp(current_group_relationship->item.group_relationship.group_name, message->to_group_name)==0&&
           strcmp(current_group_relationship->item.group_relationship.user_name, message->from_name)==0){
            fflag = 1;
            break;
        }
        current_group_relationship = current_group_relationship->next;
    }
    if(fflag == 1){
        message->cmd = 230;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //没有加入过该群
    //根据群名在数据库中找到群主
    char owner_name[200];  //储存群主名

    //根据群名，查询群主
    mysql_node *current_user_group = NULL;
    fflag = 0;
    current_user_group = head_user_groups;
    while(current_user_group != NULL){
        if(strcmp(current_user_group->item.user_groups.name, message->to_group_name) == 0){
            fflag = 1;
            break;
        }
        current_user_group = current_user_group->next;
    }
    if(fflag == 0){ //该群不存在
        message->cmd = 231;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }
    strcpy(owner_name, current_user_group->item.user_groups.owner);
    strcpy(message->to_name, owner_name);
    //printf("-------------owner_name: %s\n", message->to_name);
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
    printf("-------------owner_name: %s\n", message->to_name);

    send(current->date.socket, message, sizeof(MSG), 0);
}

void quit_group(int conn_fd, MSG *message){
    printf("%s 退出 %s\n", message->from_name, message->to_group_name);

    //查看是否存在该群
    //printf("AAAAAAAAAAAAAAAAA\n");
    mysql_node *current_user_group = NULL;
    int fflag = 0;
    current_user_group = head_user_groups;
    while(current_user_group != NULL){
        if(strcmp(current_user_group->item.user_groups.name, message->to_group_name) == 0){
            fflag = 1;
            break;
        }
        current_user_group = current_user_group->next;
    }
    if(fflag == 0){  //该群不存在
        message->cmd = 240;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //群主不能退出群
    if(strcmp(current_user_group->item.user_groups.owner, message->from_name) == 0){
        message->cmd = 241;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }
    //保存群主名
    strcpy(message->to_name, current_user_group->item.user_groups.owner);

    //查看申请方是否加入过该群
    mysql_node *current_group_relationship = NULL;
    fflag = 0;
    current_group_relationship = head_group_relationship;
    while(current_group_relationship != NULL){
        if(strcmp(current_group_relationship->item.group_relationship.group_name, message->to_group_name) == 0&&
           strcmp(current_group_relationship->item.group_relationship.user_name, message->from_name) == 0){
            fflag = 1;
            break;
        }
        current_group_relationship = current_group_relationship->next;
    }
    if(fflag == 0){
        message->cmd = 242;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    char str[200];
    //在群关系表中删除记录
    sprintf(str, "delete from group_relationship where group_name = '%s' and user_name = '%s'",
                message->to_group_name, message->from_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        exit(1);
    }
    //群关系链表中做相应的操作
    mysql_node *delete_group_relationship = head_group_relationship;
    mysql_node *temp;
    while(delete_group_relationship != NULL){
        if(strcmp(delete_group_relationship->item.group_relationship.group_name, message->to_group_name)==0&&
           strcmp(delete_group_relationship->item.group_relationship.user_name, message->from_name)==0){
            if(head_group_relationship == delete_group_relationship){
                head_group_relationship = delete_group_relationship->next;
                free(delete_group_relationship);
                break;
            }
            temp->next = delete_group_relationship->next;
                break;
        }
        temp = delete_group_relationship;
        delete_group_relationship = delete_group_relationship->next;
    }

    //通知退出成功
    message->cmd = 243;
    send(conn_fd, message, sizeof(MSG), 0);


    //通知群主该成员退出
    //查看群主是否在线
    int flag = 0;
    Node *current = head;
    while(current != NULL){
        if(strcmp(current->date.name, message->to_name) == 0){
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
    message->cmd = 24;
    send(current->date.socket, message, sizeof(MSG), 0);
}

void view_group_member(int conn_fd, MSG *message){
    printf("%s 正在查询群成员\n", message->from_name);

    //查看是否存在该群
    mysql_node *current_user_group = NULL;
    int fflag = 0;
    current_user_group = head_user_groups;
    while(current_user_group != NULL){
        if(strcmp(current_user_group->item.user_groups.name, message->to_group_name) == 0){
            fflag = 1;
            break;
        }
        current_user_group = current_user_group->next;
    }
    if(fflag == 0){
        message->cmd = 2510;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //查看该人是否在群中
    mysql_node *current_group_relationship = NULL;
    fflag = 0;
    current_group_relationship = head_group_relationship;
    while(current_group_relationship != NULL){
        if(strcmp(current_group_relationship->item.group_relationship.group_name, message->to_group_name)==0&&
           strcmp(current_group_relationship->item.group_relationship.user_name, message->from_name)==0){
            fflag = 1;
            break;
        }
        current_group_relationship = current_group_relationship->next;
    }
    if(fflag == 0){
        message->cmd = 2511;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //查看群成员
    current_group_relationship = head_group_relationship;
    while(current_group_relationship != NULL){
        if(strcmp(current_group_relationship->item.group_relationship.group_name, message->to_group_name) == 0){
            strcpy(message->to_name, current_group_relationship->item.group_relationship.user_name);
            message->friend_num = current_group_relationship->item.group_relationship.type;
            send(conn_fd, message, sizeof(MSG), 0);
        }
        current_group_relationship = current_group_relationship->next;
    }
}

void break_group(int conn_fd, MSG *message){
    //查看是否存在该群
    char str[200];
    mysql_node *current_user_group = NULL;
    int fflag = 0;
    current_user_group = head_user_groups;
    while(current_user_group != NULL){
        if(strcmp(current_user_group->item.user_groups.name, message->to_group_name) == 0){
            fflag = 1;
            break;
        }
        current_user_group = current_user_group->next;
    }
    if(fflag == 0){
        message->cmd = 2520;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //查看操作者是否是群主
    if(strcmp(current_user_group->item.user_groups.owner, message->from_name) != 0){
        message->cmd = 2521;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    printf("----------%s 解散了 %s\n", message->from_name, message->to_group_name);

    //是群主，1.删除群，2.在群关系表中也删除该群
    sprintf(str, "delete from user_groups where name = '%s'", message->to_group_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        //mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }
    //群链表中做相应操作
    mysql_node *delete_user_group = head_user_groups;
    mysql_node *temp;
    while(delete_user_group != NULL){
        if(strcmp(delete_user_group->item.user_groups.name, message->to_group_name)==0){
            if(head_user_groups == delete_user_group){
                head_user_groups = delete_user_group->next;
                free(delete_user_group);
                break;
            }
            temp->next = delete_user_group->next;
                break;
        }
        temp = delete_user_group;
        delete_user_group = delete_user_group->next;
    }

    sprintf(str, "delete from group_relationship where group_name = '%s'",
                message->to_group_name);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        //mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }
    //群关系链表中做相应操作
    mysql_node *current_group_relationship = head_group_relationship;
    while(current_group_relationship != NULL){
        if(strcmp(current_group_relationship->item.group_relationship.group_name, message->to_group_name) == 0){
            mysql_node *delete_group_relationship = head_group_relationship;
            mysql_node *temp;
            while(delete_group_relationship != NULL){
                if(strcmp(delete_group_relationship->item.group_relationship.group_name, message->to_group_name)==0){
                    if(head_group_relationship == delete_group_relationship){
                        head_group_relationship = delete_group_relationship->next;
                        free(delete_group_relationship);
                        break;
                    }
                    temp->next = delete_group_relationship->next;
                    break;
                }
                temp = delete_group_relationship;
                delete_group_relationship = delete_group_relationship->next;
            }
        }
        current_group_relationship = current_group_relationship->next;
    }

    //通知解散完成
    send(conn_fd, message, sizeof(MSG), 0);
}

void set_manager(int conn_fd, MSG *message){
    //查看是否是群主
    mysql_node *current_group_relationship = NULL;
    int fflag = 0;
    current_group_relationship = head_group_relationship;
    while(current_group_relationship != NULL){
        if(strcmp(current_group_relationship->item.group_relationship.group_name, message->to_group_name)==0&&
           strcmp(current_group_relationship->item.group_relationship.user_name, message->from_name)==0&&
           current_group_relationship->item.group_relationship.type == 1){
            fflag = 1;
            break;
        }
        current_group_relationship = current_group_relationship->next;
    }
    printf("group_type = %d\n", current_group_relationship->item.group_relationship.type);
    if(fflag == 0){  //不是群主
        message->cmd = 2530;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }
    
    //查看目标用户是否是该群成员
    current_group_relationship = head_group_relationship;
    fflag = 0;
    while(current_group_relationship != NULL){
        if(strcmp(current_group_relationship->item.group_relationship.group_name, message->to_group_name)==0&&
           strcmp(current_group_relationship->item.group_relationship.user_name, message->to_name)==0){
            fflag = 1;
            break;
        }
        current_group_relationship = current_group_relationship->next;
    }
    if(fflag == 0){
        message->cmd = 2531;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }
    
    
    //设置管理员
    char str[200];
    sprintf(str, "update group_relationship set user_type = 2 where group_name = '%s' and user_name = '%s'",
                message->to_group_name, message->to_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d error\n", __LINE__-2);
        //mysql_free_result(mysql_res);
        mysql_close(&mysql);
        exit(1);
    }
    printf("[%s] %s将%s设置为管理员\n", message->to_group_name, message->from_name, message->to_name);
    current_group_relationship->item.group_relationship.type = 2;
    send(conn_fd, message, sizeof(MSG), 0);
}

void deal_friend_request_y(int conn_fd, MSG *message){
    printf("%s 同意申请\n", message->to_name);

    //查看被申请方之前有没有申请方的好友
    char str[200];
    mysql_node *current_friend = NULL;
    int flag = 0;
    current_friend = head_friends;
    while(current_friend != NULL){
        if(strcmp(current_friend->item.friends.f_friendname, message->from_name)==0 && strcmp(current_friend->item.friends.f_username, message->to_name)==0){
            flag = 1;
            break;
        }
        current_friend = current_friend->next;
    }
    //将好友关系插入到好友表，处理状态为1
    if(!flag){  //如果没有
        sprintf(str, "insert into friend values(NULL,'%s','%s',1,1)",
                    message->from_name, message->to_name);
        int ret = mysql_real_query(&mysql, str, strlen(str));
        if(ret){
            printf("line: %d", __LINE__-2);
            exit(1);
        }
        //加入链表
        mysql_node *new_friend = NULL;
        new_friend = (mysql_node*)malloc(sizeof(mysql_node));
        strcpy(new_friend->item.friends.f_friendname, message->from_name);
        strcpy(new_friend->item.friends.f_username, message->to_name);
        new_friend->item.friends.type = 1;
        new_friend->item.friends.status = 1;
        new_friend->next = NULL;
        weicha_list(&head_friends, new_friend);
    }
    if(flag){
        sprintf(str, "update friend set status = 1 where f_friendname = '%s' and f_username = '%s'",
                    message->from_name, message->to_name);
        int ret = mysql_real_query(&mysql, str, strlen(str));
        if(ret){
            printf("line: %d error\n", __LINE__-2);
            exit(1);
        }

        current_friend->item.friends.status = 1;
    }

    //将申请方的处理状态置为1;
    sprintf(str, "update friend set status = 1 where f_friendname = '%s' and f_username = '%s'",
                    message->to_name, message->from_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        //printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    //改链表中的数据
    current_friend = head_friends;
    while(current_friend != NULL){
        if(strcmp(current_friend->item.friends.f_friendname, message->to_name)==0 && strcmp(current_friend->item.friends.f_username, message->from_name)==0){
            break;
        }
        current_friend = current_friend->next;
    }
    current_friend->item.friends.status = 1;

    //找到要通知的用户节点
    flag = 0;
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
}

void deal_friend_request_n(int conn_fd, MSG *message){
    //在好友表中将申请方的记录删除
    char str[200];
    memset(str, 0, sizeof(str));
    sprintf(str, "delete from friend where f_friendname = '%s' and f_username = '%s'",
                message->to_name, message->from_name);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        mysql_close(&mysql);
        exit(1);
    }
    //在好友链表中做相应操作
    mysql_node *delete_friend = head_friends;
    mysql_node *temp;
    while(delete_friend != NULL){
        if(strcmp(delete_friend->item.friends.f_friendname, message->to_name)==0&&strcmp(delete_friend->item.friends.f_username, message->from_name)==0){
            if(head_friends == delete_friend){
                head_friends = delete_friend->next;
                free(delete_friend);
                break;
            }
            temp->next = delete_friend->next;
                break;
        }
        temp = delete_friend;
        delete_friend = delete_friend->next;
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
}

void deal_group_request_y(int conn_fd, MSG *message){
    char str[200];
    //插入群关系表
    sprintf(str, "insert into group_relationship values('%s','%s',3)",
                message->to_group_name, message->from_name);
    mysql_real_query(&mysql, str, strlen(str));
    //插入到群关系链表中
    mysql_node *new_node = NULL;
    new_node = (mysql_node*)malloc(sizeof(mysql_node));
    strcpy(new_node->item.group_relationship.group_name, message->to_group_name);
    strcpy(new_node->item.group_relationship.user_name, message->from_name);
    new_node->item.group_relationship.type = 3;
    new_node->next = NULL;
    
    weicha_list(&head_group_relationship, new_node);

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
}

void deal_group_request_n(int conn_fd, MSG *message){
    //连接数据库
    
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
    //MYSQL mysql;
    //connect_mysql(&mysql);

    //查看该群是否存在
    char str[4096];
    printf("group_name:%s | group_owner:%s\n", message->group.group_name, message->group.group_owner);
    mysql_node *current_user_group = NULL;
    int fflag = 0;
    current_user_group = head_user_groups;
    while(current_user_group != NULL){
        if(strcmp(current_user_group->item.user_groups.name, message->group.group_name) == 0){
            fflag = 1;
            break;
        }
        current_user_group = current_user_group->next;
    }
    if(fflag){  //该群已存在
        message->cmd = 50;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }

    //如果不存在，创建群
    sprintf(str, "insert into user_groups values('%s','%s')",
                message->group.group_name, message->group.group_owner);
    //printf("str: %s\n", str);
    int ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        exit(1);
    }
    //群组链表中做相应操作
    mysql_node *new_node = NULL;
    new_node = (mysql_node*)malloc(sizeof(mysql_node));
    strcpy(new_node->item.user_groups.name, message->group.group_name);
    strcpy(new_node->item.user_groups.owner, message->group.group_owner);
    new_node->next = NULL;
    weicha_list(&head_user_groups, new_node);

    //将群与群主加入群关系表中
    sprintf(str, "insert into group_relationship values('%s','%s',1)",
                message->group.group_name, message->group.group_owner);
    //printf("message->cmd = %d\n", message->cmd);
    ret = mysql_real_query(&mysql, str, strlen(str));
    if(ret){
        printf("line: %d ", __LINE__-2);
        printf("mysql_real_query(): error\n");
        //mysql_free_result(mysql_res);
        //mysql_close(&mysql);
        exit(1);
    }
    //链表中作相应操作
    mysql_node *new_node_rela = NULL;
    new_node_rela = (mysql_node*)malloc(sizeof(mysql_node));
    strcpy(new_node_rela->item.group_relationship.group_name, message->group.group_name);
    strcpy(new_node_rela->item.group_relationship.user_name, message->group.group_owner);
    new_node_rela->item.group_relationship.type = 1;
    new_node_rela->next = NULL;
    weicha_list(&head_group_relationship, new_node_rela);
    
    send(conn_fd, message, sizeof(MSG), 0);
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
    //printf("%s 正在发送文件\n", message->from_name);
    //查看是否有目标用户
    //char str[200];
    /*mysql_node *current_user = NULL;
    int fflag = 0;
    current_user = head_user;
    while(current_user != NULL){
        if(strcmp(current_user->item.user.u_name, message->to_name) == 0){
            fflag = 1;
            break;
        }
        current_user = current_user->next;
    }
    if(fflag == 0){
        message->cmd = 61;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }
    //确认是否为好友
    mysql_node *current_friend = NULL;
    fflag = 0;
    current_friend = head_friends;
    while(current_friend != NULL){
        if(strcmp(current_friend->item.friends.f_friendname, message->to_name)==0 && strcmp(current_friend->item.friends.f_username, message->from_name)==0){
            fflag = 1;
            break;
        }
        current_friend = current_friend->next;
    }
    if(fflag == 0){
        message->cmd = 60;
        send(conn_fd, message, sizeof(MSG), 0);
        return;
    }*/

    //int fd;
    //char buf[256];  //临时储存文件的路径
    
    //获得文件名
    char file_name[256];
    strcpy(file_name, get_file_name(message->file_path));
    strcpy(message->file_name, file_name);
    printf("flie name: %s\n", message->file_name);
    
    //临时存储
    //sprintf(buf, "/home/huloves/chat_room_file/%s", file_name);
    //printf("buf:%s\n", buf);
    //strcpy(message->file_name, file_name);
    //fd = open(buf, O_RDWR | O_CREAT, 0644);
    //if(fd == -1){
    //    my_err("open", __LINE__-2);
    //}

    //----------------------------------------------------------
    //找到在线目标用户
    Node *current = head;
    while(current != NULL){
        if(strcmp(current->date.name, message->to_name) == 0){
            break;
        }
        current = current->next;
    }

    //通知开始收文件
    if(current != NULL)
    send(current->date.socket, message, sizeof(MSG), 0);
    else {
        printf("data is Empty\n");
    }
    int send_size = 0;

    //接收文件
    //printf("size = %d\n", message->once_len);
    printf("-----------接受\n");
    int sum = 0;
    char *ptr = (char*)message;
    MSG message_;
    int ret = 0;
    int count = sizeof(MSG);
    while((send_size = recv(conn_fd, ptr + sum, sizeof(MSG) - sum, 0)) > 0){
        //printf("A\n");
        printf("size: %d\n", send_size);
        sum += send_size;
        printf("sum: %d\n",sum);
        if(sum == sizeof(MSG)) {
            sum = 0;
            printf("正在接收文件......size = %d\n", send_size);
            if(ntohs(message->once_len) < 1024){
                printf("接收完成...\n");
                send(current->date.socket, message, sizeof(MSG), 0);
                break;
            }
            printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa:%d\n",ntohs(message->once_len));
            send(current->date.socket, message, sizeof(MSG), 0);
        
        }
        else continue;
    }

    printf("sendsize: %d\n", send_size);

    //查看目标用户是否在线
    /*int flag = 0;
    Node *current = head;
    while(current != NULL){
        //printf("A\n");
        if(strcmp(current->date.name, message->to_name) == 0){
            flag = 1;
            break;
        }
        current = current->next;
    }

//    if(flag == 0){  //不在线
//        printf("目标不在线\n");
//        return;
//    }

    //通知目标用户有文件要接受
    sleep()
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
    
    close(fd);*/
}
