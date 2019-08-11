/*************************************************************************
	> File Name: server_deal.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月08日 星期四 16时18分30秒
 ************************************************************************/
#include "MYSQL.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>

#define LOGIN 1

static int cli_fd;

typedef struct package{
    int  type;
    int  account;
    char send_name[100];
    int  send_account;
    time_t time;
    char mes[1000];
    char mes2[1000];
} PACK;

void send_PACK(PACK pack);
void recv_PACK(int conn_fd);
void deal_regist(PACK pack);
void deal_login(PACK pack);
void deal_repass(PACK pack);
void deal(PACK pack);


void recv_PACK(int conn_fd)
{
    PACK pack;
    cli_fd = conn_fd;
    int ret;
    if((ret = recv(conn_fd, &pack, sizeof(struct package),0)) < 0){
        perror("recv");
        exit(1);
    }
    if (ret == 0)
    {
        return ;
    }
    deal(pack);
}

void send_PACK(PACK pack)
{
    int ret;
    if((ret =  send(cli_fd,&pack,sizeof(PACK),0))<0) {
        perror("server_deal ：send");
    }
}


void deal(PACK pack)
{
    int type = pack.type;
    MYSQL_init();
    switch (type)
    {
        case 1:
            deal_login(pack);
            break;
        case 2:
            deal_regist(pack);
            break;
        case 3:
            deal_repass(pack);
            break;
    }
}

void deal_login(PACK pack)
{
    PACK send_pack;
   
    int ret = MYSQL_login(pack.account, pack.mes,cli_fd);
    if(ret == 0) {
        send_pack.type = 0;
        strcpy(send_pack.mes,"登录成功");
        
    }
    else {
        send_pack.type = -1;
    }
    send_PACK(send_pack);
}

void deal_regist(PACK pack)
{
    PACK send_pack;

    int ret = MYSQL_regist(pack.mes, pack.mes2);
    if(ret != -1) {
        send_pack.type = 0;
        send_pack.account = ret;
        strcpy(send_pack.mes,"注册成功");

    }
    else {
        send_pack.type = -1;
    }
    send_PACK(send_pack);
}

void deal_repass(PACK pack)
{
    PACK send_pack;

    int ret = MYSQL_repass(pack.account, pack.mes, pack.mes2);
    if(ret != -1) {
        send_pack.type = 0;
        strcpy(send_pack.mes,"注册成功");
    }
    else {
        send_pack.type = -1;
    }
    send_PACK(send_pack);
}
