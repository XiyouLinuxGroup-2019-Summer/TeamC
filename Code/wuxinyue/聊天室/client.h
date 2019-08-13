/*************************************************************************
	> File Name: client.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月09日 星期五 09时40分31秒
 ************************************************************************/
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<time.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/epoll.h>

#define IP  "127.0.0.1" 
#define PORT 9999

typedef Msg{
    char username[20];
    char password[20];
    char type;
    char filename[20];
    char time[10];
}Msg;

