/*************************************************************************
	> File Name: processimage.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月23日 星期二 10时17分37秒
 ************************************************************************/
#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
int main(int argc,char *argv[],char *environ[])
{
    int i;

    printf("I am a process image\n");
    printf("My pid =%d,parentpid=%d\n",getpid(),getppid());
    printf("uid=%d,git=%d\n",getuid(),getgid());

    for(i=0;i<argc;i++)
    {
        printf("argv[%d]:%s\n",i,argv[i]);
    }

}
