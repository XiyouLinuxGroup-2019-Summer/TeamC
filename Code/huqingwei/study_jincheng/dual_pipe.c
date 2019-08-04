#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

//子进程读写函数
void child_rw_pipe(int readfd, int writefd){
    char *message1 = "from child process!\n";
    write(writefd, message1, strlen(message1)+1);

    char message2[100];
    read(readfd, message2, 100);
    printf("child process read from pipe:%s", message2);
}

//父进程读写函数
void parent_rw_pipe(int readfd, int writefd){
    char *message1 = "from parent process!\n";
    write(writefd, message1, strlen(message1)+1);

    char message2[100];
    read(readfd, message2, 100);
    printf("parent process read from:%s", message2);
}

int main()
{
    //创建父子进程，父进程读写，子进程读写
    //父子进程通过管道通信
    pid_t pid;
    int pipe1[2], pipe2[2];
    int ret = pipe(pipe1);
    if(ret == -1){
        perror("pipe");
        exit(1);
    }

    ret = pipe(pipe2);
    if(ret == -1){
        perror("piep");
        exit(1);
    }

    pid = fork();
    if(pid == -1){
        perror("fork");
        exit(1);
    }
    else if(pid == 0){
        //子进程关闭pipe1的写端，pipe2的读端
        close(pipe1[1]);
        close(pipe2[0]);
        child_rw_pipe(pipe1[0], pipe2[1]);
        //exit(0);
    }
    else{
        //父进程关闭pip1的读端，pipe2的写端
        ///父进程回收子进程资源
        close(pipe1[0]);
        close(pipe2[1]);
        parent_rw_pipe(pipe2[0], pipe1[1]);
        wait(NULL);
        //exit(0);
    }
    
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    return 0;
}
