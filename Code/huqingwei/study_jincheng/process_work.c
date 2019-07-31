#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

void dowork(int no){
    time_t curtime;
    //后取当前时间
    time(&curtime);
    //格式化时间
    char *pt = ctime(&curtime);
    //将时间写入文件
    int fd = open("/home/huloves/temp++++.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
    write(fd, pt, strlen(pt)+1);
    close(fd);
}

int main()
{
    pid_t pid = fork();
    if(pid > 0){
        exit(1);
    }
    else if(pid == 0){
        //变成会长 - 脱离控制终端 - 守护进程
        setsid();
        //改变进程的工作目录
        chdir("/home/huloves");
        //重置文件掩码
        umask(0);
        //关闭文件描述符
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        //执行核心操作
        //注册信号捕捉
        struct sigaction act;
        act.sa_flags = 0;
        act.sa_handler = dowork;
        sigemptyset(&act.sa_mask);
        sigaction(SIGALRM, &act, NULL);
        //创建定时器
        struct itimerval val;
        //第一次触发定时器的时间
        val.it_value.tv_usec = 0;
        val.it_value.tv_sec = 2;
        //循环周期
        val.it_interval.tv_usec = 0;
        val.it_interval.tv_sec = 1;
        setitimer(ITIMER_REAL, &val, NULL);
        //保证子进程处于运行状态
        while(1);
    }
    return 0;
}

