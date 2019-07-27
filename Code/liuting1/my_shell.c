/*************************************************************************
	> File Name: my_shell.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月24日 星期三 08时08分56秒
 ************************************************************************/

#include  <stdio.h>
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
#include <readline/readline.h>

void explain_input(char *buf,int *argcount ,char arglist[100][256]);
void do_cmd(int argcount,char (*arglist)[256]);
int  find_command(char *command);
void handler_aigint();

#define CLOSE "\001\033[0m\002"                 // 关闭所有属性
#define BLOD  "\001\033[1m\002"                 // 强调、加粗、高亮
#define BEGIN(x,y) "\001\033["#x";"#y"m\002"   


int main(int argc,char *argv[])
{

    char *buf=NULL;
    int i,argcount;;
    char arglist[100][256];
    char buffer[100];

    buf=(char *)malloc(256);//分配空间在堆上
    if(buf==NULL)
    {
        perror("malloc failed");
        exit(-1);
    }

    signal(SIGINT,handler_aigint);
    while(1)
    {
        char str[500] = {0};
        sprintf(str, BEGIN(31,42)"myshell:%s$ "CLOSE,getcwd(buffer,100));//打印提示符
       
        char  *temp;
  	    // char  str[500];
        temp = (char* )malloc(sizeof(256));  
        memset(buf,0,256);
        memset(temp,0,sizeof(temp));  //初始化空间
        
        temp = readline(str);  //调用readline库函数 实现补全
        while(strlen(temp)==0|| strlen(temp)>256)
        {
            memset(temp,0,sizeof(temp));
            temp=readline(str);
        }
        strcpy(buf, temp); 

        free(temp);

        if(strcmp("exit",buf)==0||strcmp(buf,"logout")==0)//输入exit logout 关闭终端
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
                if (*q == '\0')
                {
                    strncpy(arglist[*argcount],p,number);
                    arglist[*argcount][number]='\0';
                    *argcount=*argcount+1;
                    return;
                }
                number++;
                q++;
            }
            strncpy(arglist[*argcount],p,number);
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
    for(i=0;i<argcount;i++)
    {
        if(strlen(arg[i])==0)
        {
            arg[i]=NULL;
            break;
        }
    }

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
            }
            arg[i]=NULL;
            flag=8; 
        }
    }

        if(strcmp(arg[0],"cd") == 0){
        if(argcount == 1 || argcount == 2 && strcmp(arg[1],"~")==0||strcmp(arg[1],"-")==0)  //特殊情况判断
		{
		    chdir("/home/lt");
            return;
		}
        if(chdir(arg[1]) == -1){
            perror("chdir");
        }
        return ;
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
                if(!(find_command(arg[0])))
                {
                    printf("%s:command not found\n",arglist[0]);
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
                int pid2;
                int status2;
                int fd2;
                if((pid2=fork())<0)
                {
                     printf("fork2 error\n");
                    return ;
                }
                else if(pid2==0)
                {
                    if(!(find_command(arg[0]))&&!(find_command(argnext[0])))
                    {
                        printf("%s:command not found\n",arg[0]);
                    }
                    fd2=open("/tmp/youdonotfileknow",O_WRONLY|O_CREAT|O_TRUNC,0644);
                    dup2(fd2,1);
                    execvp(arg[0],arg);
                    exit(0);
                }
                if(waitpid(pid2,&status2,0)==-1)
                {
                     printf("wait for child process error\n");
                }

                
                if(!find_command(argnext[0]))
                {
                    printf("%s:command not found\n",argnext[0]);
                }
                fd=open("/tmp/youdonotfileknow",O_RDONLY);
                printf("%d\n",fd);
                dup2(fd,0);
                printf("%s\n",argnext[0]);

                execvp(argnext[0],argnext);
                //if(remove("/tmp/youdonotfileknow"))
                //{
                  //  printf("remove error\n");
                //}
                exit(0);

                }
            break;
        default:
            break;
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
    char *path[]={"./","/bin","/usr/bin",NULL};

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
