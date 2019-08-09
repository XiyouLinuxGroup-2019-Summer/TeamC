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

#define LOGIN      1

typedef struct{
    int  type;
    int  account;
    char send_name[100];
    int  send_account;
    time_t time;
    char mes[1000];
    char mes2[1000];
} PACK;

void send_login_PACK(int conn_fd)
{
   

    PACK pack; 
    pack.type = LOGIN;
    
    printf("请输入你的账号：");
    scanf("%d",&pack.account);
    printf("请输入你的密码：");
    scanf("%s",pack.mes);

    if(send(conn_fd, &pack, sizeof(PACK),0)<0){
        perror("send");

    }
}

