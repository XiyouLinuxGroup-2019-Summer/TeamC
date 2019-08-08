#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "chat.h"

int main(int argc, char *argv[])
{
    if(argc != 5) {
        printf("Usage: [-p] [serv_port] [-a] [serv_address]\n");
        exit(1);
    }

    int i;
    int ret;
    int conn_fd;
    int serv_port;
    struct sockaddr_in serv_addr;
    MSG message;
    
    int port = atol(argv[2]);  //保存端口
    //创建套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    //连接服务器
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));

    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    inet_pton(AF_INET, argv[5], &serv.sin_addr.s_addr);
    connect(fd, (struct sockaddr*)&serv, sizeof(serv));

    //通信
    while(1) {
        //发送数据
        memset(&message, 0, sizeof(message));
        printf("Enter you message: ");
        fgets(message.msg, sizeof(message.msg), stdin);
        send(fd, message.msg, strlen(message.msg), 0);

        //等待接收数据
        int len = recv(fd, message.msg, sizeof(message.msg), 0);
        if(len == -1) {
            my_err("recv", __LINE__-2);
        }
        else if(len == 0) {
            printf("服务器关闭了连接\n");
            break;
        }
        else {
            printf("recv buf: %s\n", message.msg);
        }
    }

    close(fd);

    return 0;
}
