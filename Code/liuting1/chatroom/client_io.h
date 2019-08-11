/*************************************************************************
	> File Name: client_io.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月08日 星期四 15时55分56秒
 ************************************************************************/

#ifndef _CLIENT_IO_H
#define _CLIENT_IO_H
int send_login_PACK(int conn_fd);
int send_regist_PACK(int conn_fd);
int send_repasswd_PACK(int conn_fd);
void *recv_PACK();
#endif
