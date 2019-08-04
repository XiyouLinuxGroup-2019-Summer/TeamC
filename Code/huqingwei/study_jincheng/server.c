#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <error.h>
#include <unistd.h>

#define FIFO_READ     "/tmp/readfifo"
#define FIFO_WRITE    "/tmp/writefifo"
#define BUF_SIZE      1024

int main()
{
    int wfd, rfd;
    char buf[BUF_SIZE];
    int len;

    umask(0);
    if(open(FIFO_WRITE, O_RDWR) == -1){
        perror("open");
        printf("line: %d\n", __LINE__-2);
        exit(1);
    }
    umask(0);
    wfd = open(FIFO_WRITE, O_WRONLY);
    if(wfd == -1){
        perror("open");
        exit(1);
    }

    while((rfd = open(FIFO_READ, O_RDONLY)) == -1){
        sleep(1);
    }

    while(1){
        printf("Server: ");
        fgets(buf, BUF_SIZE, stdin);
        buf[strlen(buf)-1] = '\0';
        if(strncmp(buf, "quit", 4) == 0){
            close(wfd);
            unlink(FIFO_WRITE);
            close(rfd);
            exit(1);
        }
        write(wfd, buf, strlen(buf));

        len = read(rfd, buf, BUF_SIZE);
        if(len > 0){
            buf[len] = '\0';
            printf("Client: %s\n", buf);
        }
    }
}
