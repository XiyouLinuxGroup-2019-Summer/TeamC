/*************************************************************************
	> File Name: client_io.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月07日 星期三 17时38分50秒
 ************************************************************************/

#include<stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>


#define LOGIN      1
#define REGIST     2
#define REPASSWD   3

typedef struct{
    int  type;
    int  account;
    char send_name[100];
    int  send_account;
    time_t time;
    char mes[1000];
    char mes2[1000];
} PACK;

int cli_fd;

pthread_cond_t cond;
pthread_mutex_t mutex;

int send_login_PACK(int conn_fd)
{

    PACK pack; 
    pack.type = LOGIN;
    cli_fd = conn_fd;

    printf("请输入你的账号：");
    scanf("%d",&pack.account);
    printf("请输入你的密码：");
    scanf("%s",pack.mes);
    getchar();    
    if(send(conn_fd, &pack, sizeof(PACK),0)<0){
        perror("send");
    }
    while(1)
    {
        int ret;
        memset(&pack, 0, sizeof(PACK));
        if((ret = recv(cli_fd, &pack,sizeof(PACK), 0))>0) {
            if(pack.type == 0) {
                printf("登录成功\n");
                return 0;
            }
            else {
                printf("登录失败");
                return -1;
            }
        }
    }
}
int send_regist_PACK(int conn_fd)
{
    PACK pack;
    pack.type = REGIST;
    cli_fd = conn_fd;

    printf("请输入你的昵称：");
    scanf("%s",pack.mes);
    printf("请输入你的密码：");
    scanf("%s",pack.mes2);
    getchar();
    if(send(conn_fd, &pack, sizeof(PACK),0)<0){
        perror("send");
    }
    while(1)
    {
        int ret;
        memset(&pack, 0, sizeof(PACK));
        if((ret = recv(cli_fd, &pack,sizeof(PACK), 0))>0) {
            if(pack.type == 0) {
                printf("申请账号成功，您的账号为：%d\n",pack.account);
                return 0;
            }
            else {
                printf("注册失败");
                return -1;
            }
        }
    }
}
int send_repasswd_PACK(int conn_fd)
{
    PACK pack;
    pack.type = REPASSWD ;
    cli_fd = conn_fd;

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
    while(1)
    {
        int ret;
        memset(&pack, 0, sizeof(PACK));
        if((ret = recv(cli_fd, &pack,sizeof(PACK), 0))>0) {
            if(pack.type == 0) {
                return 0;
            }
            else {
                printf("注册失败");
                return -1;
            }
        }
    }
}

void *recv_PACK()
{
    PACK pack;
    int ret;
    while(1)
    {

        if((ret = recv(cli_fd, &pack,sizeof(PACK), 0))>0) {
            
            //switch(type) {
                //case 1:
                    //recv_deal_PACK
            //}
            //deal(pack);
        }
    }
}
