/*************************************************************************
	> File Name: server_msg.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月08日 星期四 21时25分07秒
 ************************************************************************/

typedef struct Msg{
    int type;
    char uesrname[20];
    char password[20];
    char filename[20];
    char time[10];
}Msg;
// 处理客户端通信的函数
void *handle_client(void *v){
    int client_socket = (int)v;
    Msg msg;
    while(1) {
        // 从客户端读取一个结构体数据
        int ret = read(client_socket);
        if (ret == -1){
            perror("read\n");
            break;
        }
        // 代表客户端退出
        if (ret == 0){
            printf("客户端退出\n");
            break;
        }
        switch (msg.type) {
        }
    }
}

