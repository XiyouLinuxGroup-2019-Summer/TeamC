#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include "chat.h"

/*自定义错误处理函数*/
/*打印错误行号和错误信息，并退出程序*/
void my_err(const char *err_string, int line) {
    fprintf(stderr, "line:%d ", line);
    perror(err_string);
    exit(1);
}

/*从套接字上读取一次数据*/
/*参数：conn_fd -- 从该连接的套接字上接收数据
 *      data_buf -- 读取到的数据保存在次缓冲中
 *      len -- data_buf所指向的空间长度
 *返回值：出错返回-1， 服务器已关闭连接返回0，成功返回读取到的字节数
 */
int my_recv(int conn_fd, char *data_buf, int len) {
    static char recv_buf[BUFSIZE]; //缓冲区大小
    static char *pread;            //指向下一次读取数据的位置
    static int len_remain = 0;     //缓冲区剩余字结束
    int i;

    //如果缓冲区中没有数据，从套接字读取数据
    if(len_remain <= 0) {
        if((len_remain = recv(conn_fd, recv_buf, sizeof(recv_buf), 0)) < 0) {
            my_err("recv", __LINE__-1);
        }
        else if(len == 0) {  //客户端断开连接
            return 0;
        }
        pread = recv_buf;  //重新初始化 pread 指针
    }

    for(i=0; *pread != '\n'; i++){
        if(i > len) {
            return -1;  //防止指针越界
        }

        data_buf[i] = *pread++;
        len_remain--;
    }
    //去除标志位
    len_remain--;

    //返回读取到的字符串
    return i;
}

/*初始化套接字
 * 参数：void
 * 返回值：执行成功返回监听套接字，失败返回-1
 * */
int listen_socket() {
    struct sockaddr_in serv_addr;
    socklen_t serv_len = sizeof(serv_addr);

    //创建套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    //初始化服务器sockaddr_in
    memset(&serv_addr, 0, serv_len);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //监听本机所有的IP
    serv_addr.sin_port = htons(PORT);
    //绑定IP和端口
    int ret = bind(lfd, (struct sockaddr*)serv_len, serv_len);
    if(ret == -1) {
        my_err("bind", __LINE__-2);
    }

    ret = listen(lfd, 36);
    if(ret == -1) {
        my_err("listen", __LINE__-2);
    }

    return lfd;
}

/*接收消息包*/
int recv_message(int conn_fd, MSG *message) {
    int len = recv(conn_fd, message, sizeof(message), 0);
    if(len == -1) {
        my_err("recv", __LINE__-2);
    }
    else if(len == 0) { //客户端断开
        return 0;
    }

    return 1;
}

