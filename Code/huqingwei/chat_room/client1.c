#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <linux/socket.h>
#include <string.h>
#include <pthread.h>

#include "chat.h"

struct node{  //请求结构体
    //int cmd;
    //char time[30];
    //char from_name[20];
    //char to_name[20];
    MSG request;
    struct node *next;
};

int fd;  //用来通信的文件描述符
char myname[20]; //自己的名字

struct node *head_friend = NULL;
struct node *head_group = NULL;

pthread_mutex_t mutex;
pthread_cond_t cond;

void my_err(char *err, int line){  //错误处理函数
    printf("line: %d", line);
    perror(err);
    exit(1);
}

char *my_time(){
    char *str;
    time_t now;
    time(&now);

    str = ctime(&now);
    str[strlen(str)-1] = '\0';

    return str;
}

//尾插法
void charu_list(struct node **phead, MSG request){
    struct node *current, *new_node;
    new_node = (struct node*)malloc(sizeof(struct node));
    //init
    new_node->request = request;
    new_node->next = NULL;

    if(*phead == NULL){
        *phead = new_node;
        return;
    }

    //找到尾节点
    current = *phead;
    while(current->next != NULL){
        current = current->next;
    }

    current->next = new_node;
    return;
}

//从头删除一个节点
void del_head(struct node *phead){
    struct node *temp;

    temp = phead;
    phead = temp->next;
    
    free(temp);
}

//遍历并显示链表内容
void bianli_list(struct node *phead){
    struct node *current;
    int i = 0;
    current = phead;
    while(current != NULL){
        printf("%d [%s]想加你为好友\n", i, current->request.from_name);
        current = current->next;
        i++;
    }
}

int login_face(int conn_fd);            //登录界面
int user_face(int conn_fd);             //conn_fd  //用户界面
int reg(int conn_fd);                   //注册功能
int login(int conn_fd);                 //登录功能
int forget_password(int conn_fd);       //忘记密码
int my_friend(int conn_fd);             //我的好友功能
void *server_message(void *arg);        //线程处理函数
int choice_group(int conn_fd);          //选择群聊
int deal_request(int conn_fd);          //处理申请
int create_group(int conn_fd);          //创建群聊
int exit_face(int conn_fd);             //注销
int all_friends(int conn_fd);           //查看所有好友
int online_friends(int conn_fd);        //查看在线好友
int private_chat(int conn_fd);          //私聊功能
int add_friend(int conn_fd);            //添加好友
int friend_request(int conn_fd);        //好友申请处理
int group_request(int conn_fd);         //入群申请处理
int view_group(int conn_fd);            //查看我的群
int add_group(int conn_fd);             //申请加群
int public_chat(int conn_fd);           //群聊

int main(int argc, char * argv[])
{
    if(argc < 3)
    {
        printf("eg: ./a.out [port] [ip]\n");
        exit(1);
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    int ret;
    pthread_t pthread1;
    int port = atoi(argv[1]); //字符串变量转换成整形变量
    //创建套接字
    fd = socket(AF_INET, SOCK_STREAM, 0);

    //连接服务器
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    //serv.sin_addr.s_addr = htonl()
    //inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);
    inet_pton(AF_INET, argv[2], &serv.sin_addr.s_addr);
    ret = connect(fd, (struct sockaddr*)&serv, sizeof(serv) );
    if(ret == -1){
        perror("connect");
        printf("line: %d\n", __LINE__-3);
        exit(1);
    }

    //通信
    //显示登录界面
    while(1){
        system("clear");
        char choice[10]; //登录界面的选择
        memset(choice, 0 ,sizeof(choice));
        printf("----------------------\n");
        printf("☆       1.注册      ☆ \n");
        printf("----------------------\n");
        printf("☆       2.登录      ☆ \n");
        printf("----------------------\n");
        printf("☆       3.忘记密码  ☆ \n");
        printf("----------------------\n");
        //printf("☆       3.退出      ☆ \n");
        //printf("----------------------\n");

        while(1){
            printf("请输入你的选择：");
            scanf("%s", choice);
            while(getchar() != '\n')
                continue;
            
            if(strcmp(choice, "1")==0 || strcmp(choice, "2")==0 || strcmp(choice, "3")==0){
                break;
            }
            else{
                printf("输入错误，请按照提示输入\n");
            }
        }

        if(choice[0] == '3'){  //退出
            //close(fd);  //断开连接
            //exit(1);
            forget_password(fd);
            getchar();
            continue;
        }
        
        if(choice[0] == '1'){  //选择注册
            ret = reg(fd);
            if(ret == 1){ //注册成功
                //break;
                continue;
            }
            else{
                continue;
            }
        }
        else if(choice[0] == '2'){  //选择登录
            ret = login(fd);
            printf("ret = %d\n", ret);
            if(ret == 1){  //登陆成功
                break;
            }
            else{          //登录失败
                //printf("登录失败\n");
                continue;
                //break;
            }
            //pthread_mutex_unlock(&mutex);
        }
    }

    //登陆成功，创建用户线程，接受服务器的消息
    pthread_create(&pthread1, NULL, server_message, NULL);
    pthread_detach(pthread1);
    user_face(fd);

    return 0;
}

//线程处理函数
void *server_message(void *arg){
    //pthread_cond_wait(&cond, &mutex);
    //printf("A\n");
    
    while(1){
        MSG message;
        //char *recv_message;
        //recv_message = (char*)malloc(sizeof(message)); //每次接受信息的为一个结构体大小
        memset(&message, 0, sizeof(message));
    
        int len = recv(fd, &message, sizeof(message), 0);
        if(len == -1){
            my_err("recv", __LINE__-2);
        }
        else if(len == 0){
            printf("服务器断开连接\n");
            pthread_exit(0);
        }
        //memcpy(&message, recv_message, sizeof(message));
        //printf("Q\n");
        //printf("----message.cmd: %d\n", message.cmd);
        switch(message.cmd){
        case 111:
            printf("你还没有朋友\n");
            break;
        case 11:
            printf("好友: %s\n", message.friend_name);
            break;
        case 121:
            //printf("目前没有好友在线\n");
            printf("目前你还没有好友\n");
            break;
        case 12:
            printf("在线好友: %s\n", message.friend_name);
            break;
        case 13:
            printf("\033[0m\033[s"); //保存当前光标
            printf("\033[0m\033[4;1H");  //光标跳到第四行，第一格
            printf("%s %s说：\n", message.time, message.from_name);
            printf("%s\n", message.news);
            printf("\033[0m\033[u");
            printf("\033[0m\033[u");  //恢复光标
            fflush(stdout);   //清空I/O缓存中的标准输出缓存使光标恢复到原点
            break;
        //case -13:
          //  printf("你的好友不在线\n");
          //  break;
        case 14:                              //-----------------------------------
            printf("\n你有一个好友申请，请到消息处理\n");
            //printf("A\n");
            pthread_mutex_lock(&mutex);
            charu_list(&head_friend, message);
            pthread_mutex_unlock(&mutex);
            //printf("N\n");
            //bianli_list(head_friend);
            break;
        case 211:
            printf("你还没有加入群聊，你可以自己创建群聊\n");
            break;
        case 21:
            printf("群名: %s\n", message.group.group_name);
            break;
        case 22:
            printf("%s %s说：\n", message.time, message.from_name);
            printf("%s\n", message.news);
            break;
        case 23:
            printf("\n你有一个入群申请，请到消息处理\n");
            pthread_mutex_lock(&mutex);
            charu_list(&head_group, message);
            pthread_mutex_unlock(&mutex);
            break;
        case 311:
            printf("\n%s 已成为你的好友\n", message.to_name);
            break;
        case 321:
            printf("\n%s 你已加入该群\n", message.to_name);
            break;
        case 51:
            printf("该群名已存在\n");
            break;
        case 52:
            printf("%s 群创建成功\n", message.group.group_name);
            break;
        }
    }
}

int user_face(int conn_fd) {
    char choice[2];
    while(1){
        system("clear");
        printf("-------------------------\n");
        printf("☆       1.我的好友     ☆ \n");
        printf("-------------------------\n");
        printf("        2.选择群聊     \n");
        printf("-----------------------\n");
        printf("        3.处理申请     \n");
        printf("-----------------------\n");
        printf("        4.消息盒子     \n");
        printf("-----------------------\n");
        printf("        5.创建群聊     \n");
        printf("-----------------------\n");
        printf("        0.注销退出     \n");
        printf("-----------------------\n");
        //printf("    请输入你的选择：");

        //scanf("%s", ch);
        //while(getchar() != '\n');
    
        while(1){
            printf("   请输入你的选择：");
            scanf("%s", choice);

            //printf("choice: %s\n", choice);

            if(strcmp(choice, "1")==0||strcmp(choice, "2")==0||strcmp(choice, "3")==0||
                strcmp(choice, "4")==0||strcmp(choice, "5")==0||strcmp(choice, "0")==0){
                break;
            }
            else{
                printf("输入错误，请按照提示输入\n");
            }
        }

        switch(choice[0]){
        case '1':
            my_friend(conn_fd);
            break;
        case '2':
            choice_group(conn_fd);
            break;
        case '3':
            deal_request(conn_fd);
            break;
        case '5':
            create_group(conn_fd);
            break;
        case '0':
            //exit_face(conn_fd);
            close(conn_fd);
            exit(1);
            break;
        }
    }

    return 0;
}

int reg(int conn_fd) {
    MSG message;
    //message.type = 'r';
    message.cmd = 1;

    char buf[256];
    printf("*姓名：");
    scanf("%s", buf);
    while(getchar() != '\n');
    strcpy(message.user_infor.name, buf);

    printf("昵称：");
    scanf("%s", buf);
    while(getchar() != '\n');
    strcpy(message.user_infor.nickname, buf);

    printf("性别：");
    scanf("%s", buf);
    while(getchar() != '\n');
    strcpy(message.user_infor.sex, buf);

    printf("密码：");
    scanf("%s", buf);
    while(getchar() != '\n');
    strcpy(message.user_infor.password, buf);

    send(conn_fd, &message, sizeof(message), 0);
    recv(conn_fd, &message, sizeof(message), 0);

    printf("cmd = %d\n", message.cmd);
    //注册成功返回 1001
    if(message.cmd == 1001) {
        printf("注册成功\n");
        printf("正在返回登录界面\n");
        sleep(1);
        return 1;
    }
    //注册失败返回 -1
    else if(message.cmd == -1) {
        printf("该用户已被注册\n");
        printf("正在返回登录界面\n");
        return -1;
    }
}

int login(int conn_fd){
    MSG message;
    memset(&message, 0, sizeof(message));
    message.cmd = 2;

    char buf[256];
    printf("用户名：");
    scanf("%s", buf);

    strcpy(message.user_infor.name, buf);
    strcpy(myname, buf);

    printf("密  码：");
    scanf("%s", buf);
    
    strcpy(message.user_infor.password, buf);
    send(conn_fd, &message, sizeof(message), 0);
    recv(conn_fd, &message, sizeof(message), 0);

    //登录成功返回1002
    printf("---cmd = %d\n", message.cmd);
    if(message.cmd == 1002) {
        printf("登陆成功\n");
        //pthread_cond_signal(&cond);
        printf("M\n");
        sleep(1);
        return 1;     
    }
    //登录失败返回-2
    else if(message.cmd == -2) {
        printf("登录失败\n");
        sleep(1);
        return -1;
    }
}

int forget_password(int conn_fd){
    MSG message;
    int len = sizeof(message);
    memset(&message, 0, sizeof(message));

    message.cmd = 0;
    
    char buf[200];
    memset(buf, 0, sizeof(buf));
    printf("姓名：");
    scanf("%s", buf);
    strcpy(message.user_infor.name, buf);
    printf("请输入预留的电话号码：");
    scanf("%s", buf);
    strcpy(message.user_infor.telephone, buf);

    if(send(conn_fd, &message, sizeof(message), 0) != len){
        my_err("send", __LINE__-1);
    }
    recv(conn_fd, &message, sizeof(message), 0);

    if(message.cmd == 01){
        printf("该账户未注册\n");
    }
    else if(message.cmd == 02){
        printf("电话号码错误\n");
    }
    else if(message.cmd == 03){
        printf("账号密码：%s\n", message.user_infor.password);
    }

    printf("按回车键返回菜单\n");
    getchar();
}

int my_friend(int conn_fd){
    char choice[10];
    
    while(1){
        memset(choice, 0, sizeof(choice));
        system("clear");
        printf("=========== %s ===========\n", myname);
        printf("-------------------------------\n");
        printf("☆      1.查看所有好友        ☆ \n");
        printf("-------------------------------\n");
        printf("☆      2.查看在线好友        ☆ \n");
        printf("-------------------------------\n");
        printf("       3.选择私聊              \n");
        printf("-------------------------------\n");
        printf("☆      4.添加好友            ☆ \n");
        printf("-------------------------------\n");
        printf("       5.删除好友              \n");
        printf("-------------------------------\n");
        printf("       0.返回上级菜单          \n");
        printf("-------------------------------\n");

        while(1){
            printf("请输入你的选择：");
            scanf("%s", choice);
            while(getchar() != '\n')
                continue;

            if(strcmp(choice, "1")==0||strcmp(choice, "2")==0||strcmp(choice, "3")==0||strcmp(choice, "4")==0||strcmp(choice, "0")==0){
                break;
            }
            else{
                printf("错误输入，请按照提示输入\n");
            }
        }

        switch(choice[0]){
        case '0':
            return 0;
        case '1':
            all_friends(conn_fd);
            break;
        case '2':
            online_friends(conn_fd);
            break;
        case '3':
            private_chat(conn_fd);
            break;
        case '4':
            add_friend(conn_fd);
            break;
        }
    }
}

int all_friends(int conn_fd){
    MSG message;
    int len = sizeof(message);
    memset(&message, 0, len);
    //message.type = 'a';
    message.cmd = 11;
    
    strcpy(message.user_infor.name, myname);

    if(send(conn_fd, &message, sizeof(message), 0) != len){
        my_err("send", __LINE__-1);
    }
    /*recv(conn_fd, &message, sizeof(message), 0);
    if(message.friend_num == 0){
        printf("你还没有朋友\n");
    }
    else{
        for(int i=0; i<message.friend_num; i++){
            recv(conn_fd, &message, sizeof(message), 0);
            printf("haoyou: %s\n", message.friend_name);
        }
    }*/
    sleep(1);
    printf("按任意键返回菜单\n");
    getchar();
}

int online_friends(int conn_fd){
    MSG message;
    int len = sizeof(message);
    memset(&message, 0, len);
    
    //message.type = 'o';
    message.cmd = 12;
    strcpy(message.user_infor.name, myname);
    if(send(conn_fd, &message, sizeof(message), 0) != len){
        my_err("send", __LINE__-1);
    }

    sleep(1);
    
    printf("按任意键返回菜单\n");
    getchar();
}

int private_chat(int conn_fd){
    MSG message;
    char buf[1024];
    char now_time[30];
    int len = sizeof(message);
    memset(&message, 0, sizeof(message));
    memset(buf, 0, sizeof(buf));
    
    message.cmd = 13;
    //memcpy(message.time, my_time(), 30);
    strcpy(message.from_name, myname);
    printf("请输入你好友的名字：");
    scanf("%s", buf);
    strcpy(message.to_name, buf);

    system("clear");

    printf("-----正在与 %s 聊天-----\n", message.to_name);
    printf("   --- quit 退出 ---\n");
    printf("=============================\n");

    while(1){
        memset(now_time, 0, sizeof(now_time));
        memset(buf, 0, sizeof(buf));
        strcpy(now_time, my_time());
        printf("\033[0m\033[16;0H");
        printf("%s %s :\n", now_time, message.from_name);
        scanf("%s", buf);
        while(getchar()!='\n')
            continue;

        if(strcmp(buf, "quit") == 0){
            break;
        }

        strcpy(message.time, now_time);
        strcpy(message.news, buf);
        send(conn_fd, &message, sizeof(message), 0);
    }

    printf("按任意键返回菜单\n");
    getchar();
}

int add_friend(int conn_fd){
    MSG message;
    char buf[1024];
    int len = sizeof(message);
    memset(&message, 0, sizeof(message));
    memset(&buf, 0, sizeof(buf));
    
    message.cmd = 14;
    strcpy(message.from_name, myname);

    printf("请输入添加的好友名字：");
    scanf("%s", buf);
    strcpy(message.to_name, buf);
    //printf("%s\n", message.to_name);
    
    if(send(conn_fd, &message, sizeof(message), 0) != len){
        printf("A\n");
        my_err("send", __LINE__-2);
        exit(1);
    }
}

int choice_group(int conn_fd){
    char choice[10];
    memset(choice, 0, sizeof(choice));

    while(1){
        system("clear");
        printf("-------------------------\n");
        printf("       1.查看我的群      \n");
        printf("-------------------------\n");
        printf("       2.群聊            \n");
        printf("-------------------------\n");
        printf("       3.申请入群        \n");
        printf("-------------------------\n");
        printf("       4.退出群聊        \n");
        printf("-------------------------\n");
        printf("       0.返回上级菜单    \n");
        printf("-------------------------\n");

        while(1){
            printf("请输入你的选择：");
            scanf("%s", choice);
            while(getchar() != '\n')
                continue;

            if(strcmp(choice, "1")==0||strcmp(choice, "2")==0||strcmp(choice, "3")==0||
                strcmp(choice, "4")==0||strcmp(choice, "0")==0){
                break;
            }
            else{
                printf("请按照提示输入\n");
            }
        }

        switch(choice[0]){
        case '0':
            return 0;
        case '1':
            view_group(conn_fd);
            break;
        case '2':
            public_chat(conn_fd);
            break;
        case '3':
            add_group(conn_fd);
            break;
        }
    }
}

int view_group(int conn_fd){
    MSG message;
    int len = sizeof(message);
    memset(&message, 0, sizeof(message));

    message.cmd = 21;
    strcpy(message.user_infor.name, myname);

    if(send(conn_fd, &message, sizeof(message), 0) != len){
        my_err("send", __LINE__-1);
    }

    sleep(1);
    printf("按回车键返回菜单\n");
    getchar();
}

int public_chat(int conn_fd){
    MSG message;
    char buf[1024];
    char now_time[30];
    int len = sizeof(message);
    memset(&message, 0, sizeof(message));
    memset(buf, 0, sizeof(buf));

    message.cmd = 22;

    strcpy(message.from_name, myname);
    printf("请输入群聊的名称：");
    scanf("%s", buf);
    strcpy(message.to_group_name, buf);

    printf("--------正在 %s 中聊天--------\n", message.to_group_name);
    printf("     ---- quit 退出 ----\n");
    printf("================================\n");

    while(1){
        memset(now_time, 0, sizeof(now_time));
        memset(buf, 0, sizeof(buf));
        strcpy(now_time, my_time());
        printf("%s %s :\n", now_time, message.from_name);
        scanf("%s", buf);
        while(getchar() != '\n')
            continue;

        if(strcmp(buf, "quit") == 0){
            break;
        }

        strcpy(message.time, now_time);
        strcpy(message.news, buf);
        if(send(conn_fd, &message, sizeof(message), 0) != len){
            my_err("send", __LINE__-1);
        }
    }

    printf("按回车键返回菜单\n");
    getchar();
}

int add_group(int conn_fd){
    MSG message;
    char buf[1024];
    int len = sizeof(message);
    memset(&message, 0, sizeof(message));
    memset(buf, 0, sizeof(buf));

    message.cmd = 23;
    strcpy(message.from_name, myname);

    printf("请输入你想要加入的群名：");
    scanf("%s", buf);
    strcpy(message.to_name, buf);

    if(send(conn_fd, &message, sizeof(message), 0) != len){
        my_err("send", __LINE__-1);
    }
}

int deal_request(int conn_fd){
    char choice[10];
    //MSG message;
    //int len = sizeof(message);
    //memset(&message, 0, sizeof(message));
    memset(choice, 0, sizeof(choice));

    while(1){
        system("clear");
        printf("--------------------------\n");
        printf("       1.好友申请\n");
        printf("       2.入群申请\n");
        printf("       3.文件消息\n");
        printf("       0.返回上级菜单\n");
        printf("--------------------------\n");

        while(1){
            printf("请输入你的选择：");
            scanf("%s", choice);
            while(getchar() != '\n')
                continue;

            if(strcmp(choice, "1")==0||strcmp(choice, "2")==0||strcmp(choice, "3")==0||strcmp(choice, "0")==0){
                break;
            }
            else{
                printf("请按照提示输入\n");
            }
        }

        switch(choice[0]){
        case '0':
            return 0;
        case '1':
            friend_request(conn_fd);
            break;
        case '2':
            group_request(conn_fd);
            break;
        }
    }
}

int friend_request(int conn_fd){
    MSG message;
    int num;
    int len = sizeof(message);
    memset(&message, 0, sizeof(message));
    //message.cmd = 31;
    

    //如果没有申请，直接返回
    if(head_friend == NULL){
        printf("目前你还没有还有申请\n");
        printf("\n按回车键返回菜单\n");
        getchar();
        return 0;
    }
    
    //显示第一个好友申请
    struct node *current;
    current = head_friend;
    printf("B\n");
    printf("from_name: %s\n", current->request.from_name);
    message = current->request;
    printf("A\n");
    printf("%s 想要加你为好友\n", current->request.from_name);

    printf("-------同意输入'y', 不同意输入'n'-------\n");
    char choice[2];
    //scanf("%s", choice);
    while(1){
        printf("请输入你的选择：");
        scanf("%s", choice);
        if(strcmp(choice, "y")==0||strcmp(choice, "n")==0){
            break;
        }
        else{
            printf("请按照提示输入\n");
        }
    }

    switch(choice[0]){
    case 'y':  //同意申请
        message.cmd = 311;
        if(send(conn_fd, &message, sizeof(message), 0) != len){
            my_err("send", __LINE__-1);
        }
        printf("%s 和你已经成为好友\n", message.from_name);
        pthread_mutex_lock(&mutex);
        del_head(head_friend);
        pthread_mutex_unlock(&mutex);
        break;
    case 'n':  //拒绝申请
        message.cmd = 312;
        if(send(conn_fd, &message, sizeof(message), 0) != len){
            my_err("send", __LINE__-1);
        }
        printf("你已经拒绝了申请\n");
        pthread_mutex_lock(&mutex);
        del_head(head_friend);
        pthread_mutex_unlock(&mutex);
        break;
    }

    printf("按任意键返回菜单\n");
    getchar();
}

int group_request(int conn_fd){
    MSG message;
    int num;
    int len = sizeof(message);
    memset(&message, 0, sizeof(message));
    //message.cmd = 31;
    
    //如果没有入群申请，直接返回
    if(head_group == NULL){
        printf("目前没有入群申请\n");
        printf("按回车键返回菜单\n");
        getchar();
        return 0;
    }

    //显示第一个群申请
    struct node *current;
    current = head_group;
    //printf("B\n");
    printf("from_name: %s\n", current->request.from_name);
    message = current->request;
    //printf("A\n");
    printf("%s 想要申请入群\n", current->request.from_name);

    printf("-------同意输入'y', 不同意输入'n'-------\n");
    char choice[2];
    //scanf("%s", choice);
    while(1){
        printf("请输入你的选择：");
        scanf("%s", choice);
        if(strcmp(choice, "y")==0||strcmp(choice, "n")==0){
            break;
        }
        else{
            printf("请按照提示输入\n");
        }
    }

    switch(choice[0]){
    case 'y':  //同意申请
        message.cmd = 321;
        if(send(conn_fd, &message, sizeof(message), 0) != len){
            my_err("send", __LINE__-1);
        }
        //printf("%s 和你已经成为好友\n", message.from_name);
        printf("%s 已加入群聊\n", message.from_name);
        pthread_mutex_lock(&mutex);
        del_head(head_group);
        pthread_mutex_unlock(&mutex);
        break;
    case 'n':  //拒绝申请
        message.cmd = 322;
        if(send(conn_fd, &message, sizeof(message), 0) != len){
            my_err("send", __LINE__-1);
        }
        printf("你已经拒绝了申请\n");
        pthread_mutex_lock(&mutex);
        del_head(head_group);
        pthread_mutex_unlock(&mutex);
        break;
    }

    printf("按任意键返回菜单\n");
    getchar();
}

//创建群聊
int create_group(int conn_fd){
    MSG message;
    char buf[20];
    int len = sizeof(MSG);
    memset(&message, 0, sizeof(message));
    memset(buf, 0, sizeof(buf));

    message.cmd = 5;

    strcpy(message.group.group_owner, myname); //群主
    printf("请输入群名：");
    scanf("%s", buf);
    while(getchar() != '\n')
        continue;

    strcpy(message.group.group_name, buf);  //群名
    
    if(send(conn_fd, &message, sizeof(message), 0) != len){
        my_err("send", __LINE__-1);
    }
    
    sleep(1);
    printf("按任意键返回菜单\n");
    getchar();
    //getchar();
}
