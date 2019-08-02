#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <linux/socket.h>


int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        printf("eg: ./a.out port\n");
        exit(1);
    }
    int port = atoi(argv[1]);
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
        char buf[1024]; 
        printf("请输入要发送的字符串：\n");
        fgets(buf, sizeof(buf), stdin);
        write(fd, buf, strlen(buf));

        //等待接收数据
        int len = read(fd, buf, sizeof(buf));
        if(len == -1)
        {
            perror("read error");
            exit(1);
        }
        else if(len == 0)
        {
            printf("服务器端关闭了连接\n");
            break;
        }
        else
        {
            printf("recv buf: %s\n", buf);
        }
    }

    close(fd);

    return 0;
}
