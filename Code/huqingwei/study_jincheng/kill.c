#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

int main()
{
    pid_t pid = fork();
    if(pid > 0){
        while(1){
            printf("parent process, %d\n", getpid());
            sleep(1);
        }
    }
    //chile process
    //杀死父进程
    else if(pid == 0){
        sleep(2);
        kill(getppid(), SIGKILL);
    }

    return 0;
}

