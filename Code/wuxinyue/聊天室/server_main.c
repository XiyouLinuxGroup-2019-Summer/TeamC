/*************************************************************************
	> File Name: server_main.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月09日 星期五 10时05分57秒
 ************************************************************************/
#include<stdio.h>
#include "server_accept.h"
#include "_recv.h"

int main(){
	int server_socket = Init_socket();
	int client_socket = My_accept(server_socket);
    _recv(client_socket);
}

