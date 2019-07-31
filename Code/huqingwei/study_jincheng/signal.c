#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void myfunc(int no){
    printf("catch you signal: %d\n", no);
}

int main()
{
    //捕捉crtl+c
    //注册捕捉函数
    signal(SIGINT, myfunc);

    while(1){
        sleep(1);
        printf("hello\n");
    }

    return 0;
}

