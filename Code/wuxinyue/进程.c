/*#include <stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
int main()
{
    pid_t pid;

    pid = fork();
    switch(pid){
    case 0:
        while(1) {
            printf ("a background process,childpid:%d,parentpid:%d\n",getpid(),getppid());
            sleep(3);
        }
    case -1:
        perror("process creation failed\n");
        exit(-1);
    default:
        printf ("i am parent process,my pid is %d\n",getppid());
        exit(0);
    }
    return 0;
}*/
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main(int argc, char *argv[], char ** environ )
{
    int i;
    printf("i am a process image\n");
    printf("my_pid=%d,paretpid=%d\n",getpid(),getppid());
    printf("uid=%d,gid=%d\n",getuid(),getgid());
    for(i=0; i<argc; i++)
        printf("argv[%d]:%d\n",argv[i]);
                    
}
