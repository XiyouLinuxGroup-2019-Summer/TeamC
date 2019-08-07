#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    char buffer[32];
    int ret = 0;
    int host = 0;
    int network = 0;
    unsigned address = 0;
    struct in_addr in;

    in.s_addr = 0;

    //输入以一个.分割的字符串形式的IP地址
    printf("please input your IP address:");
    fgets(buffer, 31, stdin);
    buffer[31] = '\0';

    //实例使用 inet_aton()函数
    if((ret = inet_aton(buffer, &in)) == 0){
        printf("inet_aton: \t invalid address\n");
    }
    else{
        printf("inet_aton:\t0x%x\n", in.s_addr);
    }

    //实例使用 inet_addr() 函数
    if((address = inet_addr(buffer)) == INADDR_NONE){
        printf("inet_addr: \t invalid address\n");
    }
    else{
        printf("inet_addr:\t0x%x\n", address);
    }

    //实例使用inet_ntoa()函数
    if(inet_ntoa(in) == NULL){
        printf("inet_ntoa: \t invalid address\n");
    }
    else{
        printf("inet_ntoa:\t%s\n", inet_ntoa(in));
    }

    return 0;
}

