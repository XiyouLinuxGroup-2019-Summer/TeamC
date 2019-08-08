#ifndef  __chat_H
#define  __chat_H
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9876
#define BUFSIZE 1024

typedef struct msg {
    //struct sockaddr_in addr_in; //发送源的地址
    char msg[1024];  //消息内容
    char name[20];   //用户账户
    char password[20];  //用户密码
    char flag[2];   //用户状态（1在线0下线）
    char toname[20];  //发送目的账户名
    char fromname[20];   //来源
    int cmd;  //消息类型
}MSG;

/*错误处理函数*/
void my_err(const char *err_string, int line);

/*接收数据后，处理数据让数据以 \0 结尾*/
int my_recv(int conn_fd, char *data_buf, int len);

/*接收消息包*/
int recv_message(int conn_fd, MSG *message);

/*初始化套接字i
 * 参数：void
 * 返回值：执行成功返回监听套接字，失败返回-1
 * */
int listen_socket();



#endif
