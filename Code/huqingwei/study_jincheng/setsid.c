#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int main()
{
    //创建一个会话
    //将子进程 - 会长
    pid_t pid = fork();

    if(pid >0){
        exit(1);
        //kill(getpid(), SIGKILL);
        //raise(SIGKILL);
        //abort();
    }
    else if(pid == 0){
        //变成会长
        //会长就是一个守护进程
        setsid();
        //让子进程一直活着
        while(1);
    }

    return 0;
}

