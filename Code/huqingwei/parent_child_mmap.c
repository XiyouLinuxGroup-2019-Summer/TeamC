#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

int main()
{
    //打开一个文件
    int fd = open("text.txt", O_RDWR);
    int len = lseek(fd, 0, SEEK_END);
    
    //创建内存映射区
    void *ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    close(fd);

    //创建子进程
    pid_t pid = fork();
    if(pid == -1){
        perror("fork error");
        exit(1);
    }
    if(pid > 0){
        strcpy((char*)ptr, "hello,world");
        //回收
        wait(NULL);
    }
    else if(pid == 0){
        //读数据
        printf("%s\n", (char*)ptr);
    }
    

    return 0;
}

