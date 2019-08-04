#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<dirent.h>
#include<pthread.h>
#include<pwd.h>
#include<grp.h>
#include <readline/readline.h>
#include <readline/history.h>
#define normal              0   //一般的命令
#define out_redirect        1   //输出重定向
#define in_redirect         2   //输入重定向
#define have_pipe           3   //命令中有管道
void print();                        //打印提示符
void get_input(char *buf);         //得到输入的命令
void explain_input( char *buf,int *argcount,char arglist[][256] );      //对输入的命令进行解析
void do_cmd( int argcount,char arglist[][256] ); //执行命令
int find_command( char *command);     //查找命令中的可执行程序
int main( int argc,char **argv )
{
    signal( SIGINT,SIG_IGN );    //忽略中断信号
    int i,argcount=0; 
    char arglist[100][256];
    char **arg=NULL;
    char *buf=NULL;

    while(1)
    {
        //将buf所指空间清0
        buf=(char *)malloc(sizeof(char)*256);
        memset( buf,0,256 );

        print();
       // get_input( buf ); 

        buf=readline(" ");            //使用readline()

        int len=strlen(buf);          //判断是否输入了命令
        if( !len )
        {
            continue;
        }

        if( *buf )
        {
            add_history(buf);
        } 

        //若输入的命令为exit或logout则退出本程序
        if( strcmp( buf,"exit" ) == 0 || strcmp( buf,"logout" ) == 0 )
        {
            break;
        }
        for( i=0;i<100;i++ )
        {
            arglist[i][0]='\0';
        }
        argcount=0;

        explain_input( buf, &argcount,arglist );  //命令解析
        do_cmd(argcount,arglist);                 //命令执行
        if( buf != NULL )
        {
            free( buf );
            buf = NULL;
        }
    }

    exit(0);
}
void print()                    //打印提示符
{
    uid_t uid;
    struct passwd *pw;

    uid = getuid();
    pw = getpwuid( uid );

    char *buf=NULL;
    buf=(char *)malloc(sizeof(char)*100);

    getcwd(buf,100);
    printf( "\033[;34m %s\033[0m",pw->pw_name);
   // printf( "/home/yue" );
    printf( "\033[;36m%s\033[0m $ ",buf );

}

void explain_input( char *buf, int *argcount, char arglist[100][256] )  //解析buf中的命令 遇到\n结束
{
    char *p = buf;
    char *q = buf;
    int number = 0;

    while(1)
    {
        if( p[0] == '\0' )
        {
            break;
        }
        if( p[0] == ' ' )
            p++;
        else
        {
            q=p;
            number=0;
            while( (q[0] != ' ') && (q[0] != '\0') )
            {
                number++;
                q++;
            }
            strncpy( arglist[*argcount],p,number );
            arglist[*argcount][number] = '\0';
            *argcount += 1;
            p=q;
        }
    }
}

void do_cmd( int argcount,char arglist[][256] )
{
    int flag=0;
    int how=0;                         //用于指示命令中是否含有 >  <  |
    int background = 0;                     //标识命令中是否有后台运行标识符 &
    int status, i, fd;
    char *arg[argcount+1], *argnext[argcount+1], *file;
    pid_t pid; 

    //将命令取出
    for( i=0;i<argcount;i++ )
    {
        arg[i] = ( char * )arglist[i];
    }
    arg[argcount] = NULL;

    //查看命令行是否有后台运行符
    for( i=0;i<argcount;i++ )
    {
        if( strncmp( arg[i],"&",1 ) == 0 )
        {
            if( i == argcount-1 )             //&位置在命令最后
            {
                background=1;
                arg[argcount-1]=NULL;
                break;
            }
            else
            {
                printf( "Command error!\n" );
                return ;
            }
        }
    }

    if( strncmp( arg[0],"cd",2)==0 )  // cd命令
    {
        char temp[10] = "/home/lzj";
        if( arg[1] == NULL || !strcmp(arg[1],"~" ))
        {
            if( (chdir(temp) ))
            {
                printf("chdir ~ error     \n");
            }
            return ;
        }
        else
        {
            if(arg[1][0] == '~')
            {
                char test[20],test_1[20];
                strcpy(test,arg[1]);
                strncpy(test_1,test+1,sizeof(test)-1);
                strcat(temp,test_1);
                chdir(temp);
            }
            else if( chdir(arg[1]) )
            {
                printf( "error chdir" );
            }
            return ;
        }
    }

    for( i=0; arg[i] != NULL; i++ )
    {
        if( strcmp( arg[i],">" ) == 0 )  //命令中有输出重定向
        {
            flag++;
            how = out_redirect;
            if( arg[i+1] == NULL ) //如果 > 是最后一个
            flag++;
        }
        if( strcmp( arg[i],"<" ) == 0 )  //命令中有输入重定向
        {
            flag++;
            how = in_redirect;
            if( i == 0 )
            {
                flag++;
            }
        }
        if( strcmp( arg[i],"|" ) == 0 )  //命令中有管道
        {
            flag++;
            how=have_pipe;
            if( arg[i+1] == NULL )
            {
                flag++;
            }
            if( i == 0 )
            {
                flag++;
            }
        }
    }

    //flag>1 说明命令中含有多个>, <, | 符号，或者命令格式不对，对此我的shell表示力不从心
    if( flag>1 )
    {
        printf("Command error!\n");
        return ;
    }
    if( how == out_redirect )           //命令只含有一个输出重定向符号
    {
        for( i=0; arg[i] != NULL; i++ )
        {
            if( strcmp( arg[i],">" )==0 )
            {
                file = arg[i+1];
                arg[i] = NULL;
            }
        }
    }
    if( how == in_redirect )            //命令只含有一个输入重定向符号
    {
        for( i=0; arg[i] != NULL; i++ )
        {
            if( strcmp(arg[i],"<") == 0 )
            {
                file = arg[i+1];
                arg[i]=NULL;
            }
        }
    }
    if( how == have_pipe )              //命令中只有一个管道符号
    {
        //把管道后面的部分存入argnext，管道后面那部分是一个可执行的shell命令
        for( i=0; arg[i] != NULL; i++ )
        {
            if( strcmp( arg[i],"|" )== 0 )
            {
                arg[i] = NULL;
                int j;
                for( j=i+1; arg[i] != NULL; j++ )
                {
                    argnext[j-i-1] = arg[j];
                }
                argnext[j-i-1] = arg[j];   //存一个NULL
                break;
            }
        }
    }
    if( (pid = fork()) <0 ) //执行命令都在子进程中
    {
        printf( "process creation failed!\n" );
        return ;
    }
    switch( how )
    {
        case 0:
        {
            //pid=0说明是子进程，在子进程执行输入的命令，命令中不含>, <, |
            if( pid == 0 )
            {
                if( !(find_command( arg[0] )) )
                {
                    printf("%s :Command not found!\n",arg[0]);
                    exit(0);
                }
                execvp( arg[0],arg );
                exit(0);
            }
            break;
        }
        case 1:
        {
            //输入的命令中含有输出重定向
            if( pid == 0 )
            {
                if( (!find_command( arg[0] )) )
                {
                    printf( "%s : Command not found!\n",arg[0] );
                    exit(0);
                }
                fd = open( file,O_RDWR | O_CREAT | O_TRUNC, 0644 ); //可读可写，不存在创建，可写打开时，文件清空  0644的0表示十进制
                dup2( fd,1 );    //指定新文件描述符为1
                execvp( arg[0],arg );
                exit(0);
            }
            break;
        }
        case 2:
        {
            //输入的命令中含有输出重定向
            if( pid == 0 )
            {
                if( !(find_command(arg[0])) )
                {
                    printf( "%s : Command not find!\n",arg[0] );
                    exit(0);
                }
                fd = open( file,O_RDONLY ); //只读打开
                dup2( fd,0 );
                execvp( arg[0],arg );
                exit(0);
            }
            break;
        }
        case 3:
        {
            //输入的命令中含有管道符
            if( pid == 0 )
            {
                int pid2;
                int status2;
                int fd2;

                if( ( pid2=fork() )<0 )   //在子进程中在创建一个子进程
                {
                    printf( "process Creation failed!\n" );
                    exit(0);
                }
                else if( pid == 0 )
                {
                    if( !(find_command(arg[0])) )
                    {
                        printf( "%s : Command not found!\n",arg[0] );  //
                        exit(0);
                    }
                    fd2 = open( "/tmp/transfer",O_WRONLY | O_CREAT | O_TRUNC,0644 );
                    dup2( fd2,1 );
                    execvp( arg[0],arg );
                    exit(0);
                }

                /*if( waitpid( pid2,&status2,0 ) == -1 )
                {
                    printf( "wait for child process error!\n" );
                }*/
                if( !(find_command(arg[0])) )
                {
                    printf( "%s : Command not find!\n",arg[0] );
                    exit(0);
                }
                fd2 = open( "/tmp/transfer",O_RDONLY );
                dup2( fd2,0 );
                execvp( argnext[0],argnext );

                if( remove( "/tmp/transfer" ) )
                    printf( "remove error\n" );

                exit(0);
            }
            break;
        }

        default:
            break;
    }

    //若命令中有&，表示后台执行，父进程直接返回，不等待子进程结束
    if( background == 1 )
    {
        printf( "process id : %d\n",pid );
        return ;
    }
    if( waitpid( pid,&status,0 ) == -1 )
    {
        printf( "wait for child process error\n" );

    }
}


//查找命令中的可执行程序
int find_command( char *command )
{
    DIR *dp;
    struct dirent *dirp;
    char *path[]={ "./", "/bin", "/usr/bin", NULL };

    if ( strncmp( command,"./",2 ) == 0 )  //如果命令是./fork之类，使指针跳过目录指向命令
    {
        command=command+2;
    }

    //分别在当前目录，/bin和/usr/bin目录查找要执行的程序
    int i=0;
    while( path[i] != NULL )
    {
        if( (dp=opendir(path[i]) ) == NULL )
        {
            printf( "can not open /bin\n" );
        }
        while( (dirp=readdir(dp)) != NULL )
        {
            if( strcmp( dirp->d_name,command ) == 0 )
            {
                closedir( dp );
                return 1;
            }
        }
        closedir( dp );
        i++;
    }
    return 0;
}


