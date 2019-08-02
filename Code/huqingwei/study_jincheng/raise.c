#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();
    if(pid > 0){
        //父进程，回收子进程资源
        int s;
        pid_t wpid = wait(&s);
        printf("chile died pid = %d\n", wpid);
        if(WIFSIGNALED(s)){
            printf("died by signal: %d\n", WTERMSIG(s));
        }
    }
    else if(pid == 0){
        raise(SIGINT);
    }
    return 0;
}

