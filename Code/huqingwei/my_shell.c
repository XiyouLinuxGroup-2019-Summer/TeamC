#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

void print_shell();
void my_err(char *err, int line);
void my_shell_input(char *buf);
void get_mingling(char *buf, int *argc, char argv[100][256]);
void do_cmd(int argc, char argvlist[][256]);

int main(int argc, char *argv[]){
    if(argc != 1){
        printf("the program only a shell\n");
        printf("please enter shell\n");
        exit(1);
    }
    else{
        int i;
        char *buf;
        int argcount;
        char arglist[100][256];

        buf = (char*)malloc(sizeof(char)*256);
        if(buf == NULL){
            my_err("malloc", __LINE__);
        }
        
        while(1){
            memset(buf, 0, 256);   //清空buf的内容
            print_shell();  //打印my_shell
            my_shell_input(buf);  //获得用户输入
            if(strcmp(buf, "exit\n") == 0 || strcmp(buf, "logout") == 0){
                printf("over\n");
                break;
            }
            
            //初始化命令列表，命令个数
            for(i=0; i<100; i++){
                arglist[i][0] = '\0';
            }
            argcount = 0;

            //获取命令列表，命令个数
            get_mingling(buf, &argcount, arglist);
            //printf("argcount = %d\n", argcount);
            /*for(i=0; i<argcount; i++){
                printf("i = %d\t", i);
                printf("address:%p ", &arglist[i]);
                printf("argv: %s\n", arglist[i]);
            }*/

            do_cmd(argcount, arglist);
        } 
    }
}

void my_err(char *err, int line){
    printf("line:%d", line-1);
    perror("err");
    exit(1);
}

void print_shell(){
        //获得用户名和主机名
        char login_name[256];
        char computer[256];
        strcpy(login_name, getlogin());
        gethostname(computer, 256);
            
        //获得当前目录
        char cwd[80];
        getcwd(cwd, 80);
        
        //打印shell
        printf("\033[1;31m%s@%s\033[m:\033[1;34m%s\033[m$ ", login_name, computer, cwd);
}

void my_shell_input(char *buf){
    int len = 0;
    int ch;

    ch = getchar();
    while(len < 256 && ch != '\n'){
        buf[len++] = ch;
        ch = getchar();
    }

    if(len == 256){
        printf("command is too long \n");
        exit(1);
    }

    buf[len] = '\n';
    len++;
    buf[len] = '\0';
}

void get_mingling(char *buf, int *argc, char argv[100][256]){
    int i, j = 0;
    int len = 0;

    while(buf[len++] != '\0');
    len -= 1;

    //printf("len = %d", len);
    //printf("buf:%s", buf);
    
    int k;
    for(i=0; i<len; i++){
        k = 0;
        //printf("%c", buf[i]);
        if(buf[i] == ' ' || buf[i] == '\n'){   //统计命令行参数个数
            (*argc)++;
            //printf("argc = %d\n", *argc);
            //printf("i = %d\n", i);
            while(j<i){   //解析命令行参数
                //printf("a");
                argv[*(argc)-1][k++] = buf[j];
                j++;
            }
            argv[(*argc-1)][j+1] = '\0';
            j++;
            //printf("argv: %s\n", argv[(*argc-1)]);
        }
    }
}

void do_cmd(int argc, char argvlist[][256]){
    int i;
    int status;
    int flag_pipe = 0;  //管道的标识

    for(i=0; i<argc; i++){   //遍历是否有|
        //printf("argv: %s\n", argvlist[i]);
        if(strcmp(argvlist[i], "|") == 0){
            flag_pipe = 1;
            break;
        }
    }

    if(flag_pipe == 1){
        int fd_pipe[2];
        int ret = pipe(fd_pipe); //打开一个管道
        if(ret == -1){
            my_err("pipe", __LINE__);
        }
        
        char canshu_father[100][256]; //储存父进程的参数
        i=0;
        int father_num = 0;
        while(strcmp(argvlist[i], "|") != 0){
            strcpy(canshu_father[i], argvlist[i]);
            i++;
            father_num++;
        }

        char canshu_son[100][256];   //储存子进程的参数
        int j=0;
        printf("i = %d\n", i);
        for(i; i<argc; i++){
            if(strcmp(argvlist[i], "|") == 0){
                continue;
            }
            strcpy(canshu_son[j++], argvlist[i]);
        }

        for(i=0; i<father_num; i++){
            printf("cnashu_father: %s\n", canshu_father[i]);
        }

        for(i=0; i<j; i++){
            printf("canshu_son: %s\n", canshu_son[i]);
        }

        

        close(fd_pipe[0]);
        close(fd_pipe[1]);
    }
}
