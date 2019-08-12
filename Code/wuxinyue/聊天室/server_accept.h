/*************************************************************************
	> File Name: server_accept.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月12日 星期一 10时02分13秒
 ************************************************************************/

#ifndef _SERVER_ACCEPT_H
#define _SERVER_ACCEPT_H

#define LISTEN_NUM 5
#define PORT 9999
#define IP "127.0.0.1"
#define EPOLL_MAX 1000
#define LISTENMAX 1000

int Init_socket();
int My_accept(int server_socket);

#endif
