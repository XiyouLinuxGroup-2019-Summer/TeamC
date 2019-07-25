/*************************************************************************
	> File Name: my_shell.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月24日 星期三 08时08分56秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>

void print_prompt();
void get_input(char *buf);
void explain_input(char *,int *,char a[][]);
void do_cmd(int,char a[][]);
int  find_command(char *);


int main(int argc,char *argc[])
{

    char *buf=NULL;
    int i,argcount;;
    char arglist[100][256];

    buf=(char *)malloc(256);//分配空间在堆上
    if(buf==NULL)
    {
        perror("malloc failed");
        exit(-1);
    }


    while(1)
    {
        memset(buf,0,256);
        print_prompt();//打印提示符
        get_input(buf);//将命令行存入buf数组
        if(strcmp("exit\n",buf)==0||strcmp(buf,"logout\n")==0)//输入exit logout 关闭终端
        {
            break;
        }
        for(i=0;i<100;i++)//初始化数组
        {
            arglist[i][0]='\0';
        }
        argcount=0;
        explain_input(buf,&argcount,arglist);//解析命令行
        do_cmd(argcount,arglist);//执行要输入的命令

    }
    if(buf !=NULL)//释放内存
    {
        free(buf);
        buf=NULL:
    }
    exit(0);
}
void myshell()
{
    printf("myshell$$ ");

}


void get_input(char *buf)
{
    int len=0;
    int ch;

    ch=getchar();
    while(len<256 && ch !='\n')
    {
        buf[len++]=ch;
        ch=getchar();
    }

    if(len==256)
    {
        printf("command is too long\n");
        exit(-1);
    }

    buf[len]='\n';
    len++;
    buf[len]='\0';
}

void explain_input(char *buf,int *argcount,char arglist[100][256])//argcount地址传递
{
    char *p=buf;
    char *q=buf;
    int number=0;
    while(1)
    {
        if(p[0]=='\n')
        {
            break;
        }

        if(p[0]==' ')
        {
            p++;
        }

        else
        {
            q=p;
            number=0;
            while(q[0]!=' ' && q[0]!='\n')
            {
                number++;
                q++;
            }
            strncpy(arglist[*argcount],p,number+1);
            arglist[*argcount][number]='\0';
            *argcount=*argcount+1;
            p=q;
            
        }
    }

}

void do_cmd(int argcount,char arglist[100][256])
{
    int flag=0;
    
}
