#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <linux/socket.h>
#include <string.h>

struct yonghu_infor {
    char name[20];  //姓名
    char login_id[20];  //登录账号
    char nickname[20];  //昵称
    char sex[5];        //性别
    char password[20];  //密码
};

typedef struct msg {
    //struct sockaddr_in addr_in; //发送源的地址
    char msg[1024];  //消息内容
    struct yonghu_infor user_infor;   //用户账户
    char flag[2];   //用户状态（1在线0下线）
    char toname[20];  //发送目的账户名
    char fromname[20];   //来源
    int cmd;  //消息类型
}MSG;

void *recv_infor(void *arg);
void *send_request(void *arg);


//登录界面
int login_face(int conn_fd);
//注册功能
int reg(int conn_fd);



int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        printf("eg: ./a.out port\n");
        exit(1);
    }
    int port = atoi(argv[1]); //字符串变量转换成整形变量
    //创建套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    //连接服务器
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    //serv.sin_addr.s_addr = htonl()
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);
    connect(fd, (struct sockaddr*)&serv, sizeof(serv) );

    //通信
    while(1)
    {
        //发送数据
        MSG message;
        memset(&message, 0, sizeof(message));
        
        int ret;
        ret = login_face(fd);
        if(ret == 2) {
            continue;
        }
        else if(ret == 3) {
            break;
        }
    }

    close(fd);

    return 0;
}

int login_face(int conn_fd) {
    char ch[2];
    int ret;

    printf("-------------------------\n");
    printf("          1.注册\n");
    printf("          2.登录\n");
    printf("          3.退出\n");
    printf("-------------------------\n");
    printf("       输入你的选择：\n");

    scanf("%c", ch);
    while(getchar() != '\n');  //清空缓冲区

    switch(ch[0]) {
    case '1':
        reg(conn_fd);
        break;
    case '2':
        //login(conn_fd);
        return 2;
    case '3':
        return 3;
    }
}

int reg(int conn_fd) {
    MSG message;
    message.cmd = 1;

    char buf[256];
    printf("*姓名：");
    scanf("%s", buf);
    while(getchar() != '\n');
    strcpy(message.user_infor.name, buf);

    printf("*登录账号：");
    scanf("%s", buf);
    while(getchar() != '\n');
    strcpy(message.user_infor.login_id, buf);

    printf("昵称：");
    scanf("%s", buf);
    while(getchar() != '\n');
    strcpy(message.user_infor.nickname, buf);

    printf("性别：");
    scanf("%s", buf);
    while(getchar() != '\n');
    strcpy(message.user_infor.sex, buf);

    printf("密码：");
    scanf("%s", buf);
    while(getchar() != '\n');
    strcpy(message.user_infor.password, buf);

    send(conn_fd, &message, sizeof(message), 0);
    recv(conn_fd, &message, sizeof(message), 0);

    //注册成功返回 1
    if(message.cmd == 1001) {
        printf("注册成功\n");
        printf("正在返回登录界面\n");
        printf(".");
        sleep(1);
        printf(".");
        sleep(1);
        printf(".\n");
        sleep(1);
        return 1;
    }
    //注册失败返回 -1
    else if(message.cmd == -1) {
        printf("该用户已被注册\n");
        printf("正在返回登录界面\n");
        printf(".");
        sleep(1);
        printf(".");
        sleep(1);
        printf(".\n");
        sleep(1);
        return -1;
    }
}

