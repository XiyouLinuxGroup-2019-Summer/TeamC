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

    printf("pipe[0] = %d\n", fd[0]);
    printf("pipe[1] = %d\n", fd[1]);

    close(fd[0]);
    close(fd[1]);

    return 0;
}
