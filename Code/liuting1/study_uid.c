/*************************************************************************
	> File Name: study_uid.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月03日 星期六 10时41分54秒
 ************************************************************************/

#include<stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    int fd;

    printf ("uid study:\n");
    printf ("Process's uid = %d,euid = %d\n",getuid(),geteuid());

    if((fd = open("test.c",O_RDWR))==-1)
    {
        printf ("Open failure,errno is %d: %s\n",errno,strerror(errno));

    }
    else
    {
        printf("Open successfully\n");
    }

    close(fd);
    exit(0);
}
