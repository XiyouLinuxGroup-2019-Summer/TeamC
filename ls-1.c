#include <sys/stat.h>                                      
#include <fcntl.h>      
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#define PARAM_NONE 0
#define PARAM_A 1
#define PARAM_L 2
#define PARAM_R 3
#define MAXROWLEN 80//一行显示的最多字符数
int g_leave_len=MAXROWLEN;
int g_maxlen;
void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);
}
void dispaly_attribute(struct stat buf)
{
    struct passwd *ptr;
    struct group *str;
    int i;
    char buff[10];
    char buf_time[32];
    /*得到文件类型*/
    switch(buf.st_mode & S_IFMT)//按位&获取文件基本属性
    {
       case S_IFIFO:
          buff[0] = 'f';
          break;
       case S_IFDIR:
          buff[0] = 'd';
          break;
       case S_IFSOCK:
          buff[0] = 's';
          break;
       case S_IFBLK:
          buff[0] = 'b';
          break;
       case S_IFLNK:
          buff[0] = 'l';
          break;
    }
    /*得到文件拥有者的权限*/
    if(buf.st_mode & S_IRUSR)
    {
        buff[1]='r';
    }
    else 
    {
        buff[1]='-';
    }
    if(buf.st_mode & S_IWUSR)
    {
        buff[2]='w';
    }
    else 
    {
        buff[2]='-';
    }
    if(buf.st_mode & S_IXUSR)
    {
        buff[3]='X';
    }
    else
    {
        buff[3]='-';
    }
    /*得到文件拥有者所属组的权限*/
    if(buf.st_mode & S_IRGRP)
    {
        buff[4]='r';
    }
    else 
    {
        buff[4]='-';
    }
    if(buf.st_mode & S_IWGRP)
    {
        buff[5]='w';
    }
    else 
    {
        buff[5]='-';
    }
    if(buf.st_mode & S_IXGRP)
    {
        buff[6]='X';
    }
    else
    {
        buff[6]='-';
    }
    /*得到其他人的权限*/
    if(buf.st_mode & S_IROTH)
    {
        buff[7]='r';
    }
    else
    {
        buff[7]='-';
    }
    if(buf.st_mode & S_IWOTH)
    {
        buff[8]='w';
    }
    else
    {
        buff[8]='-';
    }
    if(buf.st_mode & S_IXOTH)
    {
        buff[9]='x';
    }
    else
    {
        buff[9]='-';
    }
    for(i=0;i<9;i++)
    {
        printf("%c",buff[i]);//循环打印
    }
/*通过uid和gid找到用户名和组名*/
/*uid找到对应的用户名是可以通过getpwuid这个系统调用获取，这个系统调用接受一个uid返
 回一个passwd的结构体，这个结构体成员如下:
char *pw_name;
uid_t pw_uid;
gid_t pw_gid; 
char *pw_dir;
char *pw_shell;
gid对应的组名同样可以找到一个系统调用来获取，通过man我发现可以通过getgrgid()系统调用传入一个gid
返回这个gid对应的一个group结构体，这个结构体成员如下:
char *gr_name;
gid_t gr_gid; 
char **gr_mem;*/
    ptr=getpwuid(buf.st_uid);//调用函数获取文件所有者指针
    str=getgrgid(buf.st_gid);//调用函数获取文件所有者指针
    printf("\t%s\t%s",ptr->pw_name,str->gr_name);
    printf(" %d",buf.st_nlink);
    printf("\t%d",buf.st_size);
    strcpy(buf_time,ctime(&buf.st_mtime));
    buf_time[strlen(buf_time)-1]='\0';//去掉换行符
    printf(" %s",buf_time);
}
/*在没有使用-l选项时 打印一个文件名 打印时上下行对齐*/
void display_single(char *name)
{
    int i,len;
    if(g_maxlen>g_leave_len)//如果最大文件名的字数大于行剩余数 换行
    {
        printf("\n");
        g_leave_len=MAXROWLEN;
    }
    len=strlen(name);
    len=g_maxlen-len;//len最终要等于该文件名的字数与最大文件名字数的差值 以便补上空格对齐
    printf("%-s",name);
    for(i=0;i<len;i++)
    {
        printf(" ");
    }
    printf(" ");//这个空格是每个文件名之间的空格
    g_leave_len-=(g_maxlen+2);//每打印一次会消耗两个空格加最大文件名的字数
}
void display_dir(int flag_param,char *path)
{
    DIR *dir;
    int count=0;
    char filename[256][50];//用二维数组保存文件名
    char temp[50];
    struct dirent *ptr;
    /*获取指定目录下文件总数和最长的文件名*/
    dir=opendir(path);
    if(dir==NULL)
    {
        my_err("opendir",__LINE__);
    }
    while(ptr=readdir(dir)!=NULL)
    {
        if(g_maxlen<strlen(ptr->d_name))
            g_maxlen=strlen(ptr->d_name);
        count++;
    }
    closedir(dir);
    if(count>256)
        my_err("too many files under this dir",__LINE__);
    int i,j,len=strlen(path);
    /*获取该目录下所有的文件名*/
    dir=opendir(path);
    for(i=0;i<count;i++)
    {
        ptr=readdir(path);
        if(ptr==NULL)
            my_err("readdir",__LINE__);
        strncpy(filename[i],path,len);
        filename[i][len]='\0';
        strcat(filename[i],ptr->d_name);
        filename[i][len+strlen(ptr->d_name)]='\0';
    }
    //对文件排序
    for(i=0;i<count-1;i++)
    {
        for(j=0;j<count-i-1;j++)
        {
            if(strcmp(filename[j],filename[j+1])>0)
            {
                strcpy(temp,filename[j+1]);
                temp[(strlen(filename[j+1]))]='\0';
                strcpy(filename[j+1],filename[j]);
                filename[j+1][strlen(filename[j])]='\0';
                strcpy(filename[j],temp);
                filename[j][strlen(temp)]='\0';
            }
        }
    }
    for(i=0;i<count;i++)
        display(flag_param,filename[i]);
    closedir(dir);
}
int display_R( char *pathname1 )
{
    struct stat     buf;
    DIR             *dir;
    struct dirent   *ptr;
    printf("\n%s:\n",pathname1);
    dir = opendir( pathname1 );
    while( ( ptr = readdir( dir )) != NULL )
    {
        if(ptr->d_name[0] != '.')
        {
            display_single( ptr->d_name );
        }
    }
    closedir( dir );
    dir = opendir( pathname1 );
    while(( ptr = readdir( dir )) != NULL)
    {
        if(ptr->d_name[0] != '.')
        {
            stat( ptr->d_name, &buf );
            if( S_ISDIR( buf.st_mode ) )
            {
                char *temp = pathname1;
                strcat( temp, "/" );
                strcat( temp, ptr->d_name );
                display_R(temp);
            }
        }
    }
    closedir( dir );
    return 0;
}
void display(int flag,char *pathname)//pathname是绝对路径
{
    int i,j;
    char name[50];
    struct stat buf;
    /*从路径中解析出文件名*/
    for(i=0,j=0;i<strlen(pathname);i++)
    {
        if(pathname[i]=='/')
        {
            j=0;
            continue;
        }
        name[j++]=pathname[i];//这样的话 最后就只能保存下来最后一个\后面的字符 也就是文件名
    }
    name[j]='\0';
    if(lstat(pathname,&buf)==-1)
    {
        my_err("stat",__LINE__);
    }
     switch(flag)
    {
        case PARAM_NONE://没有参数
            if(name[0]!='.')
            break;
        case PARAM_A://-a参数
            display_single(name);
            break;
        case PARAM_L://-l参数 每个文件单独占一行 显示文件的详细属性信息
            if(name[0]!='.')
            {
                display_attribute(buf);
                printf(" %-s\n",name);
            }
            break;
        case PARAM_R://-R
            if(name[0]!='.')
            {
               display_single(name);
            }
            break;
        case PARAM_A+PARAM_R:
            display_single(name);
            break;
        case PARAM_L+PARAM_R:   
            if(name[0]!='.')
            {
                display_attribute(buf);
                printf(" %-s\n",name);
            }
            break;
        case PARAM_A+PARAM_L:
            display_attribute(buf);
            printf(" %-s\n",name);
            break;
        case PARAM_R+PARAM_L+PARAM_A:
            display_attribute(buf);
            printf(" %-s\n",name);
            break;
        default:break;
    }
}    
int main(int argc,char **argv)
{
    int i,j,k,num;
    char path[PATH_MAX+1];
    char param[32];//保存命令行参数 目标文件和目录名不在此列
    int flag_param=PARAM_NONE;//参数种类
    struct stat buf;
    j=0;
    num=0;
    for(i=1;i<argc;i++)
    {
        if(argv[i][0]=='-')
        {
            for(k=1;k<strlen(argv[i]);k++,j++)
            {
                param[j]=argv[i][k];
            }
            num++;
        }
    }
    //只支持参数a l r 如果含有其他选项就报错
    for(i=0;i<j;j++)
    {
        if(param[i]=='a')
        {
            flag_param |= PARAM_A;
            continue;
        }
        else if(param[i]=='l')
        {
            flag_param |= PARAM_L;
            continue;
        }
        else if(param[i]=='R')
        {
            flag_param |= PARAM_R;
            continue;
        }
        else
        {
            printf("my_ls:invalid options %-c\n",param[i]);
            exit(1);
        }
    }
    param[j]='\0';
    //如果没有输入文件或目录 就显示当前目录
    if((num+1)==argc)
    {
        strcpy(path,"./");
        path[2]='\0';
        display_dir(flag_param,path);
        return 0;
    }
    i=1;
    do
    {
        //如果不是目标文件名或目录 解析下一个命令行参数
        if(argv[i][0]=='-')
        {
            i++;
            continue;
        }
        else
        {
            strcpy(path,argv[i]);
            if(stat(path,&buf)==-1)
            {
            my_err("stat",__LINE__);
            }
            if(S_ISDIR(buf.st_mode))//如果argv[i]是一个目录
            {
                 if(path[strlen(argv[i])-1]!='/')
                 {
                     path[strlen(argv[i])]='/';
                     path[strlen(argv[i+1])]='\0';
                 }
                 else
                 {
                     path[strlen(argv[i])]='\0';
                 }
                 if(flag_param==4 ||flag_param==5 || flag_param==6 || flag_param==7)
                 {
                     dispaly_R(flag_param,path);
                 }
                 else
                 {
                     display_dir(flag_param,path);
                 }
                 i++;
            }
            else
            {
                display(flag_param,path);
                i++;
            }
        }
    }while(i<argc);
    return 0;
}
        
    
