#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int fd;
    
    fd = open("temp", O_CREAT | O_RDWR, 0644);
    if(fd == -1){
        printf("line:%d\n", __LINE__-2);
        perror("open");
        exit(1);
    }
    
    pid = fork();
    if(pid == -1){
        printf("line:%d\n", __LINE__-2);
        perror("fork error");
    }

    if(pid > 0){
        char *p = "hello world!";
        write(fd, p, strlen(p)+1);
        close(fd);
    }
    else if(pid == 0){
        sleep(1);
        char buf[4096];
        lseek(fd, 0, SEEK_SET);
        int len = read(fd, buf, sizeof(buf));
        printf("%s\n", buf);
    }

    return 0;
}
