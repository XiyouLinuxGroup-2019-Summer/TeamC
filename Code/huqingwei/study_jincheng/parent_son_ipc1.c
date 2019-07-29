#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

int main()
{
    //打开管道
    int fd[2];
    int ret = pipe(fd);
    if(ret == -1){
        perror("pipe error");
        exit(1);
    }

    //创建两个子进程
    //第一个子进程 ps aux
    //第二个子进程 gerp "bash"
    //父进程进行资源回收
    pid_t pid;
    int i;
    int num = 2;

    for(i=0; i<num; i++){
        pid = fork();
        if(pid == -1){
            perror("fork error");
            exit(1);
        }
        else if(pid == 0){
            break;
        }
    }

    if(i == 0){
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execlp("ps", "ps", "aux", NULL);
        perror("execlp");
        exit(1);
    }
    else if(i == 1){
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execlp("grep", "grep", "--color=auto", "bash", NULL);
        perror("execlp error");
        exit(1);
    }
    else if(i == num){
        int status;
        pid_t wpid;

        close(fd[0]);
        close(fd[1]);
        while((wpid = wait(&status)) != -1);/*{
            if(wpid == 0){
                continue;
            }
            printf("------chile died pid = %d\n", wpid);
            if(WIFEXITED(status)){
                printf("-------return value %d\n", WEXITSTATUS(status));
            }
            else if(WIFSIGNALED(status)){
                printf("-------died by signed%d\n", WTERMSIG(status));
            }
        }*/
    }

    return 0;
}

