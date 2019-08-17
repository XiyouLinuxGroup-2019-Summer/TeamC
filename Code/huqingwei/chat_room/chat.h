#ifndef  __chat_H
#define  __chat_H
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9876
#define BUFSIZE 1024

struct yonghu_infor {
    char name[20];  //姓名
    char login_id[20];  //登录账号
    char nickname[20];  //昵称
    char sex[5];        //性别
    char password[20];  //密码
    char telephone[30]; //电话号码
};

struct group{
    char group_name[20];  //群名
    char group_owner[20];  //群主名
    int member_num;  //群成员数
};

typedef struct msg {
    //struct sockaddr_in addr_in;       //发送源的地址
    //char msg[1024];                     //消息内容
    struct yonghu_infor user_infor;     //用户账户
    char to_name[20];                   //发送目的账户名
    char to_group_name[20];             //发送目的的群名
    char from_name[20];                 //来源
    char time[30];                      //时间
    struct group group;                 //群消息，群信息
    //struct yonghu_infor user;           //用户信息
    char news[1024];
    int friend_num;                     //好友个数
    char friend_name[20];               //好友名字
    int cmd;                            //登录注册消息类型
    char type;                          //消息类型
}MSG;

/*struct message_box{
    char type;  //消息类型
    char from[20];  //消息来源
    char to[20];  //消息目标
    char time[30];  //时间
    struct group group;  //群消息，群信息
    struct yonghu_infor user;  //用户信息
    char news[1024];
};*/

/*错误处理函数*/
//void my_err(const char *err_string, int line);

/*接收数据后，处理数据让数据以 \0 结尾*/
//int my_recv(int conn_fd, char *data_buf, int len);


/*接收消息包*/
//int recv_message(int conn_fd, MSG *message);

/*初始化套接字i
 * 参数：void
 * 返回值：执行成功返回监听套接字，失败返回-1
 * */
//int listen_socket();

/*用户选择功能*/
//void cmd_server(int conn_fd);

/*注册*/
//int reg(int conn_fd);

/*登录*/
//void login(int conn_fd);

#endif
