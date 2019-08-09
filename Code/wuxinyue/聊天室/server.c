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

#define LISTEN_NUM 5
#define  PORT 9999
#define IP  "127.0.0.1" 
#define EPOLL_MAX 1000
#define LISTENMAX 1000

typedef struct msg {
    char username[20];
    char password[20];
    char type;
    char filename[20];
    char time[10];
}Msg;

struct epoll_event ev,events[LISTENMAX];

int Init_socket() {

	// 初始化套接字 返回监听套接字
    // 创建一个套接字
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_socket == -1){
        perror("socket\n");
        return -1;
    }
	// 重新绑定
	int optval = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int)) <0 ){
		perror("setsockopt\n");
		return -1;
	}
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);
    int  ret = bind(server_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (ret == -1){
        perror ("bind\n");
        return -1;
    }
    ret = listen(server_socket, LISTEN_NUM);
    if (ret == -1){
        perror("listen\n");
        return -1;
    }
    printf("等待客户端连接\n");
	return server_socket;
}

// 处理客户端的连接 返回与客户端通信的套接字
int My_accept(int server_socket) {
	// 生成epoll句柄
	int epoll_fd = epoll_create(EPOLL_MAX);
	// 设置与要处理事件相关的文件描述符
	ev.data.fd = server_socket;
	// 设置要处理的文件类型 连接到达有数据来临
	ev.events = EPOLLIN;
	// 注册epoll事件
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD,server_socket, &ev);
	struct sockaddr_in client_addr;
	int len = sizeof(client_addr);
	int client_socket;
	while(1) {
        int fd_num = epoll_wait(epoll_fd, events, EPOLL_MAX, 1000 );
	    for (int i=0; i< fd_num; i++){
		    if (events[i].data.fd = server_socket){
                client_socket = (accept(server_socket, (struct sockaddr *)&client_addr, &len));
                if (client_socket == -1){
				    perror("accept\n");
				    return -1;
				 }
			printf("成功接收一个客户端:%s\n", inet_ntoa(client_addr.sin_addr));
			ev.data.fd = client_socket;
			ev.events = EPOLLIN;
		    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &ev);
		    }
			else if (events[i].events & EPOLLIN){
			}
		}
	}
	return client_socket;
}

int  _recv(int client_socket, Msg msg){
    int ret = recv(client_socket, &msg, sizeof(Msg),0);
    if (ret < 0){
        perror("recv\n");
        // return -1;
    }
}


int main(){
    Msg msg;
	int server_socket = Init_socket();
	int client_socket = My_accept(server_socket);
    _recv(client_socket, msg);
}

