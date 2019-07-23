#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

int main()
{
    int fd[2];
    int ret = pipe(fd);
    if(ret == -1){
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        exit(1);
    }

    //父进程 ps aux
    //子进程 grep "bush"
    if(pid > 0){
        //写管道操作，关闭读端
        close(fd[0]);
        //重定向，STDOUT -> 管道的写端
        dup2(fd[1], STDOUT_FILENO);
        //执行ps aux
        execlp("ps", "ps", "aux", NULL);
        perror("execlp");
        exit(1);
    }
    else if(pid == 0){
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execlp("grep", "grep", "bash", "--color=auto", NULL);
        perror("execlp");
        exit(1);
    }

    printf("pipe[0] = %d\n", fd[0]);
    printf("pipe[1] = %d\n", fd[1]);

    close(fd[0]);
    close(fd[1]);

    return 0;
}

