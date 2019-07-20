#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    int n = 5;

    pid = fork();
    switch(pid){
    case 0:
        while(n--){
            printf("A background process, PID:%d\t, ParentID:%d\n", 
                    getpid(), getppid());
            sleep(3);
        }
    case -1:
        perror("Process creation failed\n");
        exit(-1);
    default:
        sleep(1);
        printf("I am parent process,my pid is %d\n", getpid());
        exit(0);     
    }

    return 0;
}
