/*NORMAL命令中有问题*/
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
#include <signal.h>

#define NORMAL        0    //普通命令
#define OUT_REDIRECT  1    //输出重定向
#define IN_REDIRECT   2    //输入重定向
#define HAVE_PIPE     3    //含有管道命令

void print_shell();
void my_err(char *err, int line);
void my_shell_input(char *buf);
void get_mingling(char *buf, int *argc, char argv[100][256]);
void do_cmd(int argc, char argvlist[][256]);
int find_command(char *command);
void stop(int signo){
    //printf("^c\n");
    return;
}

int main(int argc, char *argv[]){
        int i;
        char *buf;
        int argcount;
        char arglist[100][256];

        signal(SIGINT, stop);

        buf = (char*)malloc(sizeof(char)*256);
        if(buf == NULL){
            my_err("malloc", __LINE__);
        }
        
        while(1){
            memset(buf, 0, 256);   //清空buf的内容
            //printf("buf:%s\n", buf);
            print_shell();  //打印my_shell
            my_shell_input(buf);  //获得用户输入
            if(strcmp(buf, "exit\n") == 0 || strcmp(buf, "logout") == 0){
                printf("exit\n");
                break;
            }
            
            //初始化命令列表，命令个数
            int j;
            for(i=0; i<100; i++){
                //arglist[i][0] = '\0';
                for(j=0; j<256; j++){
                    arglist[i][j] = '\0';
                }
            }
            argcount = 0;

            /*if(strcmp(arglist[0], "^C") == 0){
                continue;
            }*/

            //获取命令列表，命令个数
            get_mingling(buf, &argcount, arglist);

            /*for(i=0; i<argcount; i++){
                printf("i = %d\t", i);
                //printf("address:%p ", &arglist[i]);
                printf("argv: %s\n", arglist[i]);
            }*/
            if(strcmp(arglist[0], "\0") == 0){
                continue;
            }
            
            do_cmd(argcount, arglist);
        } 
}

void my_err(char *err, int line){
    //printf("line:%d", line-1);
    //perror(err);
    //printf("\n");
    printf("may be need install\n");
    exit(1);
}

void print_shell(){
        //获得用户名和主机名
        char login_name[256];
        char computer[256];
        char home_dir[256];
        char cwd[256];
        strcpy(login_name, getlogin());
        gethostname(computer, 256);
            
        //获得当前目录
        char buf[256];
        sprintf(home_dir, "/home/%s", login_name);
        getcwd(buf, 256);
        int home_dir_len = strlen(home_dir);

        int i, k = 0;
        int cwd_len = strlen(buf);
        //printf("cwd_len = %d\n", cwd_len);
        //printf("home_dir_len = %d\n", home_dir_len);
        for(i=home_dir_len; i<cwd_len; i++){
            //printf("%c", buf[i]);
            cwd[k++] = buf[i];
        }
        cwd[k] = '\0';
        //printf("%s\n", cwd);

        //打印shell
        printf("\033[1;31m%s@%s\033[m:\033[1;34m~%s\033[m$$ ", login_name, computer, cwd);
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
    
    /*if(buf[0] = '\0'){
        return;
    }*/
    
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
    int flag = 0; //只允许由一个<,>,|
    int status;
    int how = 0;  //命令中由 <,>,|
    int background = 0; //判断是否有 &
    char *file; //重定向后面的文件
    char *arg[argc+1];
    char *argnext[argc+1];
    pid_t pid;
    int fd;

    for(i=0; i<argc; i++){
        arg[i] = (char*)argvlist[i];
    }
    arg[argc] = NULL;

    /*判断是否为cd命令*/ 
    if(strcmp(arg[0], "cd") == 0){
        if(argc == 1){  //如果只有cd参数，改变当前工作目录到家目录
            char login_name[256];
            char user_dir[256];
            //strcpy(login_name, getlogin()); //get login_name
            sprintf(user_dir, "/home/%s", getlogin()); //拼接目录，得到家目录
            //printf("user_dir: %s\n", user_dir);
            chdir(user_dir);
            return;
        }
        /*如果参数大于2，显示参数过多*/
        else if(argc > 2){
            printf("my_bash: cd: 参数太多\n");
            return;
        }
        char chpath[256];
        strcpy(chpath, arg[1]);
        int ret = chdir(chpath);
        if(ret == -1){
            perror("cd");
        }
        return;
    }

    //查看命令中是否有 &
    for(i=0; i<argc; i++){
        if(strncmp(arg[i], "&", 1) == 0){
            if(i == argc-1){
                background = 1;
                arg[argc-1] = NULL;
                break;
            }
            else{
                printf("woring command!\n");
                return;
            }
        }
    }

    for(i=0; arg[i]!=NULL; i++){
        if(strcmp(arg[i], ">") == 0){
            flag++;
            how = OUT_REDIRECT;
            if(arg[i+1] == NULL){
                flag++;
            }
        }
        if(strcmp(arg[i], "<") == 0){
            flag++;
            how = IN_REDIRECT;
            if(i == 0){
                flag++;
            }
        }
        if(strcmp(arg[i], "|") == 0){
            flag++;
            how = HAVE_PIPE;
            if(arg[i+1] == NULL){
                flag++;
            }
            if(i == 0){
                flag++;
            }
        }
    }

    if(flag > 1){
        printf("wrong aommand.\n");
        return;
    }

    if(how == OUT_REDIRECT){
        for(i=0; arg[i]!=NULL; i++){
            if(strcmp(arg[i], ">") == 0){
                file = arg[i+1];
                arg[i] = NULL;
            }
        }
    }
    if(how == IN_REDIRECT){
        for(i=0; arg[i]!=NULL; i++){
            if(strcmp(arg[i], "<") == 0){
                file = arg[i+1];
                arg[i] = NULL;
            }
        }
    }
    if(how == HAVE_PIPE){
        for(i=0; i<argc; i++){
            if(strcmp(argvlist[i], "|") == 0){
                arg[i] = NULL;
                int j;
                for(j=i+1; arg[j]!=NULL; j++){
                    argnext[j-i-1] = arg[j];
                }
                argnext[j-i-1] = arg[j];
                break;
            }
        }
    }

    pid = fork();
    if(pid == -1){
        my_err("fork", __LINE__);
    }
    
    switch(how){
    case NORMAL:
        if(pid == 0){
            execvp(arg[0], arg);
            my_err("execvp", __LINE__);
            //printf("wrong command\n");
        }
        break;
    case OUT_REDIRECT:
        if(pid == 0){
            if(!(find_command(arg[0]))){
                printf("%s : command can't found\n", arg[0]);
                exit(0);
            }
            //printf("%s\n", file);
            fd = open(file, O_RDWR|O_CREAT, 0644);
            if(fd == -1){
                my_err("open", __LINE__);
            }
            dup2(fd, STDOUT_FILENO);
            execvp(arg[0], arg);
            perror("execvp");
            exit(0);
        }
        break;
    case IN_REDIRECT:
        if(pid == 0){
            if(!(find_command(arg[0]))){
                printf("%s : command cat't found\n", arg[0]);
                exit(0);
            }
            fd = open(file, O_RDONLY);
            if(fd == -1){
                my_err("open", __LINE__);
            }
            dup2(fd, STDIN_FILENO);
            execvp(arg[0], arg);
            perror("execvp");
            exit(0);
        }
        break;
    case HAVE_PIPE:
        if(pid == 0){
            int fd_pipe[2];
            int ret = pipe(fd_pipe);
            if(ret == -1){
                my_err("pipe", __LINE__);
            }

            pid_t pid2 = fork();
            if(pid2 == -1){
                my_err("fork", __LINE__);
            }

            if(pid2 == 0){
                if(!(find_command(arg[0]))){
                    printf("%s: command can't found\n", arg[0]);
                    exit(0);
                }
                close(fd_pipe[0]);
                dup2(fd_pipe[1], STDOUT_FILENO);
                execvp(arg[0], arg);
                exit(0);
            }
            else if(pid2 > 0){
                if(!(find_command(argnext[0]))){
                    printf("%s: command can't found\n", argnext[0]);
                    exit(0);
                }
                close(fd_pipe[1]);
                dup2(fd_pipe[0], STDIN_FILENO);
                execvp(argnext[0], argnext);
                exit(0);
            }
            close(fd_pipe[0]);
            close(fd_pipe[1]);
        }
        break;
    default:
        break;
    }

    if( background == 1 ){
        printf("[process id %d]\n", pid);
    }

    pid_t wpid;
    if((wpid = wait(&status)) == -1){
        printf("wait for child process error\n");
    }


    /*int flag_pipe = 0;  //管道的标识

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
    }*/
}

int find_command(char *command){
    DIR *dir;
    struct dirent *dirp;
    char *path[] = {"./", "/bin", "usr/bin", NULL};

    int i;
    for(i=0; path[i]!=NULL; i++){
        if((dir = opendir(path[i])) == NULL){
            my_err("opendir", __LINE__);
        }
        while((dirp = readdir(dir)) != NULL){
            if(strcmp(command, dirp->d_name) == 0){
                closedir(dir);
                return 1;
            }
        }
        closedir(dir);
    }
    return 0;
}
