/*************************************************************************
	> File Name: server_recv.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月12日 星期一 14时19分15秒
 ************************************************************************/
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<netinet/in.h>
#include"mysql_Init.h"
 typedef struct msg {
    char username[20];
    char password[20];
    int  type;
    char filename[20];
    char time[10];
}Msg;
Msg msg;
void _recv(int client_socket)
{
    memset(&msg, 0,  sizeof(Msg));
    int ret = recv(client_socket, &msg, sizeof(Msg), 0);
    if (ret < 0) {
        perror("server_recv \n");
    }
    switch (msg.type){
        case 1: {
            Init_mysql();
            MYSQL_regis();
            if ((send(client_socket, &msg, sizeof(Msg), 0)) < 0) {
            perror("Init_mysql_regis:send\n");
        }
        break;
        }
        case 2: {
            Init_mysql();
            MYSQL_login();
            printf("%d\n", msg.type);
            if ((send(client_socket, &msg, sizeof(Msg), 0)) < 0){
                perror("Init_mysql_Login:send\n");
            }
            break;
        }
    }
}
