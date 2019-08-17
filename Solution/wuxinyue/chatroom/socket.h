/*************************************************************************
	> File Name: socket.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 19时52分32秒
 ************************************************************************/

#ifndef _SOCKET_H
#define _SOCKET_H

#include "define.h"

int tcp_connect(const char *addr, int port);
int tcp_listen(const char *addr, int port, int backlog);
int tcp_accept(int fd, char *client_ip, int client_ip_len, int *port);
int tcp_read(int fd, char *buf, int count);
int tcp_write(int fd, const char *buf, int count);
int tcp_close(int fd);

#endif
