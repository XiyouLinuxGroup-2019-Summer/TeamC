#include "dfi.h"

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>


int cli_fd;
int account;
int send_account;

void recv_ADDFR_mess(PACK);
void recv_ADDFR_();
void recv_CHAT_FRI(PACK);
void recv_store_chat();
void recv_list_fri();
void recv_online_fri();
void recv_creat_group(PACK);
void recv_join_group(PACK);
void recv_quit_group(PACK);
void recv_dele_group(PACK);
void recv_group_mes(PACK);
void recv_CHAT_GROUP(PACK);
void recv_CHAT_FRI(PACK);
void recv_store_group(PACK);

int getch()
{
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return ch;
}

int recv_deal_PACK()
{
    PACK pack;
    while(1) {
        int ret;
        memset(&pack, 0, sizeof(PACK));
        if((ret = recv(cli_fd, &pack,sizeof(PACK), 0))>0) {
            printf("@@\n");
            if(pack.type == 0) {
                return 0;
            }
            else {
                return -1;
            }
        }
    }
}

int send_login_PACK(int conn_fd)
{

    PACK pack; 
    pack.type = LOGIN;
    cli_fd = conn_fd;

    printf("请输入你的账号：");
    scanf("%d",&pack.account);
    account = pack.account;
    printf("请输入你的密码：");
    scanf("%s",pack.mes);
    getchar();    
    if(send(conn_fd, &pack, sizeof(PACK),0)<0){
        perror("send");
    }
    while(1) {
        int ret;
        memset(&pack, 0, sizeof(PACK));
        if((ret = recv(conn_fd, &pack,sizeof(PACK), 0))>0) {
            if(pack.type == 0) {
                return 0;
            }
            else {
                return -1;
            }
        }
    }
}  

void send_regist_PACK(int conn_fd)
{
    PACK pack;
    pack.type = REGIST;

    printf("请输入你的昵称：");
    scanf("%s",pack.mes);
    printf("请输入你的密码：");
    scanf("%s",pack.mes2);
    //getchar();
    if(send(conn_fd, &pack, sizeof(PACK),0)<0){
        perror("send");
    }
    while(1) {
        int ret;
        memset(&pack, 0, sizeof(PACK));
        if((ret = recv(conn_fd, &pack,sizeof(PACK), 0))>0) {
            if(pack.type == 0) {
                printf("注册成功，您的账号为%d\n",pack.account);
                return;
            }
            else {
                return;
            }
        }
    }
}
int send_repasswd_PACK(int conn_fd)
{
    PACK pack;
    pack.type = REPASSWD ;

    printf("请输入你的账号：");
    scanf("%d",&pack.account);
    printf("请输入你的旧密码：");
    scanf("%s",pack.mes);
    printf("请输入你的新密码：");
    scanf("%s",pack.mes2);
    getchar();
    if(send(conn_fd, &pack, sizeof(PACK),0)<0){
        perror("send");
    }
    while(1) {
        int ret;
        memset(&pack, 0, sizeof(PACK));
        if((ret = recv(conn_fd, &pack,sizeof(PACK), 0))>0) {
            if(pack.type == 0) {
                return 0;
            }
            else {
                return -1;
            }
        }
    }
}


void send_exit_PACK()
{
   PACK pack;
   pack.type = EXIT;
   pack.account = account;
   if(send(cli_fd, &pack, sizeof(PACK),0)<0) {
       perror("send_exit");
   }
}

void send_addfriend_PACK()
{
    PACK pack;
    pack.type = ADD_FRIEND;
    pack.account = account;
    printf("请输入要添加的账号：");
    scanf("%d",&pack.send_account);
    getchar();
    if(send(cli_fd,&pack,sizeof(PACK),0)<0) {
        perror("send_addfriend_PACK");
    }
   
}

int send_list_fri()
{
    PACK pack;
    pack.type = LIST_FRI;
    pack.account = account;
    if(send(cli_fd, &pack, sizeof(PACK),0)<0) {
        perror("send_list_fri");
    }
}

int send_online_fri()
{
    PACK pack;
    pack.type = ONLINE_FRI;
    pack.account = account;
    if(send(cli_fd, &pack, sizeof(PACK),0)<0) {
        perror("send_list_fri");
    }
   
}

int send_chat_fri()
{
    PACK pack;
    pack.type = CHAT_FRI ;
    pack.account = account;
    printf("请输入你要发送的好友账号：");
    scanf("%d",&pack.send_account);
    send_account = pack.send_account;
    do {
        scanf("%s",pack.mes);
        getchar();
        if (send(cli_fd, &pack, sizeof(PACK),0)<0){
            perror("send");
        }
    }while (strcmp(pack.mes, "Bye") !=0 );
    send_account = 0;
}

void send_dele_fri()
{
    PACK pack;
    pack.type = DELE_FRI;
    pack.account = account;
    printf("请输入你要删除的好友账号:");
    scanf("%d",&pack.send_account);
    if (send(cli_fd, &pack, sizeof(PACK), 0)<0) {
        perror("send_dele_fri");
    }
  
}

void send_store_chat()
{
    PACK pack;
    pack.type = STORE_CHAT;
    pack.account = account;
    printf("请输入你想要查询的账号信息：");
    scanf("%d",&pack.send_account);
    getchar();
    getchar();
    if (send(cli_fd, &pack, sizeof(PACK),0)<0) 
    {
        perror("send_store_chat");
    }
    
}

void send_creat_group()
{
    PACK pack;
    pack.type = CREAT_GROUP;
    printf("请输入群名称：");
    scanf("%s",pack.mes);
    pack.account = account;
    if(send(cli_fd, &pack, sizeof(PACK), 0)<0) {
        perror("send_creat_group");
    }

}

void send_join_group()
{
    PACK pack;
    pack.type = JOIN_GROUP;
    printf("请输入你要加的群号:");
    scanf("%d",&pack.send_account);
    getchar();
    pack.account = account;
    if(send(cli_fd, &pack, sizeof(PACK), 0) < 0) {
        perror("send_join_group");
    }
}

void send_quit_group()
{
    PACK pack;
    pack.type = QUIT_GROUP;
    printf("请输入你要退出的群聊账号:");
    scanf("%d",&pack.send_account);
    getchar();
    pack.account = account;
    if(send(cli_fd, &pack, sizeof(PACK), 0) < 0) {
        perror("send_quit_group");
    }

}

void send_dele_group()
{
    PACK pack;
    pack.type = DELE_GROUP;
    printf("请输入你要删除的群号：");
    scanf("%d",&pack.send_account);
    getchar();
    pack.account = account;
    if(send(cli_fd, &pack, sizeof(PACK), 0) < 0) {
        perror("send_dele_group");
    }
}

void send_group_mes()
{
   PACK pack;
   pack.type = GROUP_MES;
   printf("请输入你要查询的群号:");
   scanf("%d",&pack.send_account);
   getchar();
   if(send(cli_fd, &pack, sizeof(PACK), 0) < 0) {
       perror("send_group_mes");
   }
}

void send_chat_group()
{
    PACK pack;
    pack.type = CHAT_GROUP;
    pack.account = account;
    printf("请输入你要嗨聊的群号：");
    scanf("%d",&pack.send_account);
    send_account = pack.send_account;
    getchar();
    do {
        scanf("%s",pack.mes);
        getchar();
        if (send(cli_fd, &pack, sizeof(PACK),0)<0){
            perror("send");
        }
    }while (strcmp(pack.mes, "Bye") !=0 );
    send_account = 0;
}

void send_find_store_g()
{
    PACK pack;
    pack.type = STORE_G;
    printf("请输入你要查询的群号：");
    scanf("%d",&pack.send_account);
    getchar();
    if(send(cli_fd, &pack, sizeof(PACK), 0) <0) {
        perror("send_file_store_g");
    }
}

void send_set_up()
{
    PACK pack;
    pack.type = SET_UP;
    pack.account = account;
    printf("请输入你要设置的群号：");
    scanf("%d",&pack.send_account);
    printf("请输入你要设置管理员的账号：");
    scanf("%s",pack.mes);
    getchar();
    if(send(cli_fd, &pack, sizeof(PACK), 0)< 0) {
        perror("send_set_up");
    }
}

void *recv_PACK()
{
    PACK pack;
    while (1) {
        if ((recv(cli_fd, &pack,sizeof(PACK), 0))>0) {
            switch (pack.type) {
                case ADD_FRIEND:
                        recv_ADDFR_mess(pack);
                        break;
                case CHAT_FRI:
                        recv_CHAT_FRI(pack);
                        break;
                case STORE_CHAT:
                        recv_store_chat();
                        break;
                case LIST_FRI:
                        recv_list_fri();
                        break;
                case ONLINE_FRI:
                        recv_online_fri();
                        break;
                case CREAT_GROUP:
                        recv_creat_group(pack);
                        break;
                case JOIN_GROUP:
                        recv_join_group(pack);
                        break;
                case QUIT_GROUP:
                        recv_quit_group(pack);
                        break;
                case DELE_GROUP:
                        recv_dele_group(pack);
                        break;
                case GROUP_MES:
                        recv_group_mes(pack);
                        break;
                case CHAT_GROUP:
                        recv_CHAT_GROUP(pack);
                        break;
                case STORE_G:
                        recv_store_group(pack);
                        break;
            }
        }
    }
}

typedef struct mess {  //加好友
    char mes[1000];
    int send_account;
    struct mess *next;
}MESS, *mess;
mess phead = NULL;
mess pend;

void recv_ADDFR_mess(PACK pack)   //bug,处理请求过程中，pthread_recv可能会存在丢包问题
{
    if(pack.account == account) {
        printf("%s\n",pack.mes);
        return;
    }
    if (phead == NULL) {
        phead = (struct mess *)malloc(sizeof(struct mess));
        phead->next = NULL;
        pend = phead;
    }
    mess pnew;
    pnew = (mess)malloc(sizeof(MESS));
    strcpy(pnew->mes , pack.mes);
    pnew->send_account = pack.account;
    pnew->next = NULL;
    pend->next = pnew;
    pend = pnew;
    printf("你有好友申请,及时前往好友申请盒子处理\n");
    
}


void recv_ADDFR_()
{
    if (phead == NULL) {
        printf("暂无好友申请消息\n");
        return;
    }
    PACK pack;
    mess p,r;
    r = phead;
    while (r->next) {
        p = r->next;
        printf("%s\n",p->mes);
        printf("是否添加？？y/n\n ");
        char a = getch();
        memset(&pack, 0, sizeof(PACK)); 
        pack.type = 41;
        if (a == 'y' || a == 'Y') {
            strcpy(pack.mes, "success");
            printf("%s\n",pack.mes);
            pack.account = account;
            pack.send_account = p->send_account;
            if(send(cli_fd, &pack, sizeof(PACK),0)<0){
                perror("send");
            }
        }
        else {
            strcpy(pack.mes, "fail");
            if(send(cli_fd, &pack, sizeof(PACK),0)<0){
                perror("send");
            }
        }
    r->next = p->next;
    free(p); 
    }
    free(r);
    phead = NULL;
}

typedef struct chat_fri {  //好友聊天
    int account;
    char mes[1000];
    char send_name[100];
    struct chat_fri *next;
}chatF;

chatF *fhead,*fend;

void recv_CHAT_FRI(PACK pack)
{
    if(pack.account == account) {
        printf("%s\n",pack.mes);
    }
    else if (pack.account == send_account) {
       printf("账号%s:  %s\n",pack.send_name,pack.mes); 
    }
    else {
        if (fhead == NULL) {
            fhead = (chatF *)malloc(sizeof(chatF));
            fhead->next = NULL;
            fend = fhead;
        }
        chatF *fnew;
        fnew = (chatF *)malloc(sizeof(chatF));
        fnew->account = pack.account;
        strcpy(fnew->send_name,pack.send_name);
        strcpy(fnew->mes , pack.mes);
        fnew->next = NULL;
        fend->next = fnew;
        fend = fnew;
        printf("你有好友消息,请及时前往私聊盒子处理\n");
    }
}

void recv_chat_fri()
{
    if (fhead == NULL) {
        printf("暂无好友消息\n");
        return;
    }
    chatF *f = fhead;
    chatF *m;
    while (f->next) {
        m = f->next;
        printf("账号%s:  %s\n",m->send_name,m->mes); 
        f->next = m->next;
        free(m); 
    }
    free(f);
    fhead = NULL;
}

void recv_store_chat()
{
    STR str;
    while(1) {
        if(recv(cli_fd, &str, sizeof(STR), 0)>0) {
            for(int i = 0; i<str.len; i++) {
                printf("send_account:%d\t", str.account[i]);
                printf("recv_account:%d\t", str.send_account[i]);
                printf("%s\n", str.mes[i]);
            }
            return ;
        }
    }
}

void recv_list_fri()
{
    fri p;
    while(1) {
        int ret;
        memset(&p, 0, sizeof(fri));
        if(recv(cli_fd, &p,sizeof(fri), 0)>0) {
            for(int i = 0; i < p.len; i++) {
                printf("account:%d  ",p.account[i]);
                printf("昵称:%s\n",p.name);
            }
            return ;
        }
    }
}

void recv_online_fri()
{
    fri p;
    while(1) {
        int ret;
        memset(&p, 0, sizeof(fri));
        if(recv(cli_fd, &p,sizeof(fri), 0)>0) {
            for(int i = 0; i < p.len; i++) {
                if(p.account[i] != 0) {
                    printf("account:%d  ",p.account[i]);
                    printf("昵称:%s\n",p.name);
                }
            }
            return ;
        }
    }
}

void recv_creat_group(PACK pack)
{
    printf("%s",pack.mes);
    printf("你的群号为%d\n",pack.account);

}

void recv_join_group(PACK pack)
{
    printf("%s\n",pack.mes);
}

void recv_quit_group(PACK pack)
{
    printf("%s\n",pack.mes);
}

void recv_dele_group(PACK pack)
{
    printf("%s\n",pack.mes);
}

void recv_group_mes(PACK pack)
{
    printf("**群名称%s\n",pack.mes);
    GROUP p;
    while(1) {
        memset(&p, 0, sizeof(GROUP));
        if(recv(cli_fd, &p,sizeof(GROUP), 0)>0) {
            for(int i = 0; i < p.len; i++) {
                if(p.account[i] != 0) {
                    printf("群员account:%d\t",p.account[i]);
                    printf("群员名称:%s\t",p.name);
                    if(p.online[i]== 1) {
                        printf("在线\t");
                    }
                    else {
                        printf("离线\t");
                    }
                    if(p.flag[i] == 1) {
                        printf("群主\n");
                    }
                    else if(p.flag[i]== 2) {
                        printf("管理员\n");
                    }
                    else {
                        printf("群员\n");
                    }
                }
            }
            return ;
        }
    }
}

typedef struct chat_group {  //好友聊天
    char group_name[100];
    char use_name[100];
    char mes[1000];
    struct chat_group *next;
}chatG;

chatG *ghead,*gend;

void recv_CHAT_GROUP(PACK pack)
{
    if(pack.account == account) {
        printf("%s\n",pack.mes);
        return ;
    }
    else if (pack.account == send_account) {
        printf("群聊名称：%s\t",pack.send_name);
        printf("群成员名称：%s\t",pack.mes);
        printf("信息：%s\n" ,pack.mes2); 
        return ;
    }
    else {
        if (ghead == NULL) {
            ghead = (chatG *)malloc(sizeof(chatG));
            ghead->next = NULL;
            gend = ghead;
        }
        chatG *gnew;
        gnew = (chatG *)malloc(sizeof(chatG));
        strcpy(gnew->group_name, pack.send_name);
        strcpy(gnew->use_name, pack.mes);
        strcpy(gnew->mes ,pack.mes2);
        gnew->next = NULL;
        gend->next = gnew;
        gend = gnew;
        printf("你有群消息,请及时前往群聊盒子处理\n");
    }
}


void recv_chat_group()
{
    if (ghead == NULL) {
        printf("暂无好友消息\n");
        return;
    }
    chatG *g = ghead;
    chatG *n;
    while (g->next) {
        n = g->next;
        printf("群聊名称：%s\n",n->group_name);
        printf("群成员名称：%s\t:",n->use_name);
        printf("信息：%s\n" ,n->mes);
        g->next = n->next;
        free(n);
    }
    free(g);
    ghead = NULL;
}

void recv_store_group(PACK pack)
{
    if(strcmp(pack.mes ,"success")!= 0)
    {
        printf("%s\n",pack.mes);
    }
    else {
        while(1) {
            int ret;
            STR_G p;
            if(recv(cli_fd, &p,sizeof(STR_G), 0)>0) {
                for(int i = 0; i < p.len; i++) {
                    if(p.usr_account[i] != 0) {
                        printf("account:%d  ",p.usr_account[i]);
                        printf("信息:%s\n",p.mes);
                    }
                }
            return ;
        }
        }
    }
}
