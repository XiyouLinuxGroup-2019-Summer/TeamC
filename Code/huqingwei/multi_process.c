#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

void my_error(char *err, int line){
    printf("line:%d\n", line-1);
    perror(err);
    exit(1);
}

int main()
{
    int i;
    int num = 3;
    pid_t pid;

    for(i=0; i<num; i++){
        pid = fork();
        if(pid == -1){
            my_error("fork error", __LINE__);
        }
        else if(pid == 0){
            break;
        }
    }

    if(i == 0){
        execlp("ps", "ps", "aux", NULL);
        my_error("execlp", __LINE__);
    }
    else if(i == 1){
        execl("./getpid_example", "getpid_example", NULL);
        my_error("execl", __LINE__);
    }
    else if(i == 2){
        execl("./error", "error", NULL);
        my_error("error", __LINE__);
    }
    else if(i == num){
        int status;
        pid_t wpid;
        //while((wpid = wait(&status)) != -1){
        while((wpid = waitpid(-1, &status, WNOHANG)) != -1){
            if(wpid == 0){
                continue;
            }
            printf("-------child died pid = %d\n", wpid);
            if(WIFEXITED(status)){
                printf("return value %d\n", WEXITSTATUS(status));
            }
            else if(WIFSIGNALED(status)){
                printf("died by signal: %d\n", WTERMSIG(status));
            }
        }
    }

    return 0;
}
