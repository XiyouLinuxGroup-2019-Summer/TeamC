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
#define register  1
#define Login 2

typedef struct msg {
    char username[20];
    char password[20];
    int  type;
    char filename[20];
    char time[10];
}Msg;


int My_connect (int client_socket) {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1){
        perror("socket\n");
        return -1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);
    int  ret = (connect(client_socket, (struct sockaddr *)&addr, sizeof(addr)));
    if (ret == -1){
        perror("connect\n");
        return -1;
    }
	return client_socket;
}

/*int Interface() {
	time_t Time = time(NULL);
	char *buf = ctime(&Time);
	system("clear");
	printf("%s", buf);
    int n;
	printf("|****************************|\n");
	printf("|          欢迎使用          |\n");
	printf("|****************************|\n");
	printf("|       1、用户注册          |\n");
	printf("|       2、用户登录          |\n");
	printf("|       3、退出              |\n");
	printf("|****************************|\n");
	printf("请输入您要进行的操作:\n");
    scanf("%d", &n);
    return n;
}*/

// 注册
int  client_regis (int client_socket){
    Msg msg;
    char username[20];
    char password[20];
    msg.type = register;
    printf("请输入用户名:");
    scanf("%s", username);
    strcpy(msg.username, username);
    printf("请输入密码:");
    scanf("%s", password);
    strcpy(msg.password, password);
    int ret = send(client_socket, &msg, sizeof(Msg), 0 );
    if (ret < 0){
        perror("send\n");
        // return -1;
    }
    sleep(1);
    memset(&msg, 0, sizeof(Msg));
    ret = recv(client_socket, &msg, sizeof(msg), 0);
    if (ret < 0){
        perror("reg_recv\n");
    }
    else {
        if (msg.type == 1001) {
            printf("%s注册成功\n", msg.username);
        }
        if (msg.type == -1) {
            printf("用户名已存在 请重新注册:\n");
        }
    }
    return client_socket;
}
int  client_Login(int client_socket){
    Msg msg;
    char username[20];
    char password[20];
    msg.type = Login;
    printf("请输入用户名:");
    scanf("%s", username);
    strcpy(msg.username, username);
    printf("请输入密码:");
    scanf("%s", password);
    strcpy(msg.password, password);
    int ret = send(client_socket, &msg, sizeof(Msg), 0 );
    if (ret < 0){
        perror("send\n");
    }
    sleep(1);
    memset(&msg, 0, sizeof(Msg));
    ret = recv(client_socket, &msg, sizeof(msg), 0);
    if (ret < 0){
        perror("reg_recv\n");
    }
    else {
        if (msg.type == 1002) {
            printf("%s登录成功\n", msg.username);
        }
        if (msg.type == -2) {
            printf("用户名不存在或密码错误 请重新登录:\n");
            client_Login(client_socket);
        }
    }
    return client_socket;
}

int main(){
    int client_socket;
    client_socket = My_connect(client_socket);
   // client_socket = client_regis(client_socket);
    client_socket = client_Login(client_socket);
}
