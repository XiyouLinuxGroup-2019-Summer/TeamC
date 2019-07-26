/*************************************************************************
	> File Name: my_shell.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月24日 星期三 08时08分56秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>


void get_input(char *buf);
void explain_input(char *buf,int *argcount ,char arglist[100][256]);
void do_cmd(int argcount,char (*arglist)[256]);
int  find_command(char *command);
void handler_aigint();


int main(int argc,char *argv[])
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

    signal(SIGINT,handler_aigint);
    while(1)
    {
        memset(buf,0,256);
        printf("myshell$$ ");//打印提示符
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
        buf=NULL;
    }
    exit(0);
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

void explain_input(char *buf,int *argcount,char  arglist[100][256])//argcount地址传递
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

void do_cmd(int argcount,char (*arglist)[256])
{
    int i,j,status,flag=0,a;
    int background=0;
    int fd;
    char *file;
    pid_t pid,pid2;
    char *arg[argcount+1],*argnext[argcount+1];

   for(i=0;i<argcount;i++)
    {
        arg[i]=(char *)arglist[i];
    }
    arg[argcount]=NULL;
    

    for (i=0;i<argcount;i++)
    {
        if(strcmp(arglist[i],"&")==0)
        {
            if(i==argcount-1)
            {
                background=1;
                arg[argcount-1] = NULL;
            }
            else
            {
                printf("& wrong");
                return ;
            }
        }
    }

    for(i=0;arg[i]!=NULL;i++)
    {
        if(strcmp(arg[i],">")==0)
        {
            if(arg[i+1]==NULL)
            {
                printf(" > no file,command wrong");
                return ;
            }
            file = arg[i+1];
            arg[i] = NULL;
            flag=1; 
        }
    }

    for(i=0;arg[i]!=NULL;i++)
    {
        if(strcmp(arg[i],">>")==0)
        {
            if(arg[i+1]==NULL)
            {
                printf(" >> no file,command wrong");
                return ;
            }
            file = arg[i+1];
            arg[i] = NULL;
            flag=2; 
        }
    }

    
    for(i=0;arg[i]!=NULL;i++)
    {
        if(strcmp(arg[i],"<")==0)
        {
            if(arg[i+1]==NULL)
            {
                printf(" < no file,command wrong");
                return ;
            }
            file = arg[i+1];
            arg[i] = NULL;
            flag=4; 
        }
    }

     for(i=0;arg[i]!=NULL;i++)
    {
        if(strcmp(arg[i],"|")==0)
        {
            if(arg[i+1]==NULL)
            {
                printf(" | no command,command wrong");
                return ;
            }
            for(j=i+1;arg[j]!=NULL;j++)
            {
                argnext[j-i-1]=arg[j];
                printf("%s\n",argnet[j-i-1])
            }
            arg[i]=NULL;
            flag=8; 
        }
    }

  
    
    
    
    
    
    if((pid=fork())<0)
    {
         printf("fork error\n");
         return ;
    }
    switch(flag)
    {
        case 0:
            if(pid==0)
            {
                if(!find_command(arg[0]))
                {
                    printf("%s:command not found");
                    exit(0);
                }
                execvp(arg[0],arg);
                exit(0);

            }
            break;
        case 1:
           if(pid==0)
            {
                if(!(find_command(arg[0])))
                {
                    printf("%s:command not found\n",arglist[0]);
                }
                fd=open(file,O_RDWR|O_CREAT|O_TRUNC,0644);
                dup2(fd,1);
                execvp(arg[0],arg);
                exit(0);
            }
            break;
        case 2:
            if(pid==0)
            {
                if(!(find_command(arg[0])))
                {
                    printf("%s:command not found\n",arglist[0]);
                }
                fd=open(file,O_RDWR|O_APPEND,0644);
                dup2(fd,1);
                execvp(arg[0],arg);
                exit(0);
            }
            break;
        case 4:
            if(pid==0)
            {
                if(!(find_command(arg[0])))
                {
                    printf("%s:command not found\n",arglist[0]);
                }
                fd=open(file,O_RDWR,0644);
                dup2(fd,0);
                execvp(arg[0],arg);
                exit(0);
            }
            break;
        case 8:
             if(pid==0)
            {
                if(!(find_command(arg[0]))&&!(find_command(argnext[0])))
                {
                    printf("%s:command not found\n",arglist[0]);
                }
                fd=open(file,O_RDWR,0644);
                dup2(fd,0);
                execvp(arg[0],arg);
                exit(0);
            }
            break;
        case 8:


   }
    if(background==1)
    {
        printf("process id %d\n",pid);
        return ;
    }
    if(waitpid(pid,&status,0)==-1)
    {
        printf("wait for child process error\n");
    }
   

}
int find_command(char *command)
{
    DIR *dir;
    struct dirent *dirp;
    char *path[]={"./","/bin","/user/bin",NULL};
    
    if(strncmp(command,"./",2)==0)
    {
        command=command+2;
    }

    int i=0;

    while(path[i]!=NULL)
    {
        if((dir =opendir(path[i]))==NULL)
        {
            printf("can not open /bin \n");
        }
        while((dirp=readdir(dir))!=NULL)
        {
            if(strcmp(dirp->d_name,command)==0)
            {
                closedir(dir);
                return 1;
            }
        }
        closedir(dir);
        i++;
    }
    return 0;
}
void handler_aigint()
{
    return ;
}
