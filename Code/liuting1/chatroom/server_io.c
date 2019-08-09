/*************************************************************************
	> File Name: server_io.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月08日 星期四 11时02分30秒
 ************************************************************************/

#include "server_deal.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct package{
    int  type;
    int  account;
    char send_name[100];
    int  send_account;
    time_t time;
    char mes[1000];
    char mes2[1000];
} PACK;

void recv_PACK(int conn_fd)
{
    PACK pack;
    int ret;
    if((ret = recv(conn_fd, &pack, sizeof(struct package),0)) < 0){
        perror("recv");
        exit(1);
    }
    if(deal(pack)==0)
    {
        

    }
}


