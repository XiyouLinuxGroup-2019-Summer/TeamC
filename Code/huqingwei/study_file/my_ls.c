#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <error.h>
#include <grp.h>
#include <grp.h>
#include <pwd.h>
#include <linux/limits.h>

void display_attribute(struct stat buf);
void file_information(char *filename);
void bianli_dir(char *dirname);
void bianli_dir_easy(char *dirname);
 
int main(int argc, char *argv[])
{
    if(1 == argc){
        bianli_dir_easy(".");
    }
    else{
        char *option = argv[1];

        if(strcmp(option, "-l") == 0){
            bianli_dir(".");
        }
    }

    return 0;
}


//获取显示文件信息并打印
void display_attribute(struct stat buf){
    char buf_time[32];
    struct passwd *psd;
    struct group *grp;

    //文件类型
    char file_type[11];
    if(S_ISLNK(buf.st_mode)){
        file_type[0] = 'l';
    }
    else if(S_ISREG(buf.st_mode)){
        file_type[0] = '-';
    }
    else if(S_ISDIR(buf.st_mode)){
        file_type[0] = 'd';
    }
    
    //所有者权限
    if(buf.st_mode & S_IRUSR){
        file_type[1] = 'r';
    }
    else{
        file_type[1] = '-';
    }
    if(buf.st_mode & S_IWUSR){
        file_type[2] = 'x';
    }
    else{
        file_type[2] = '-';
    }
    if(buf.st_mode & S_IXUSR){
        file_type[3] = 'x';
    }
    else{
        file_type[3] = '-';
    }

    //所属组权限
    if(buf.st_mode & S_IRGRP){
        file_type[4] = 'r';
    }
    else{
        file_type[4] = '-';
    }
    if(buf.st_mode & S_IWGRP){
        file_type[5] = 'w';
    }
    else{
        file_type[5] = '-';
    }
    if(buf.st_mode & S_IXGRP){
        file_type[6] = 'x';
    }
    else{
        file_type[6] = '-';
    }

    //其他用户权限
    if(buf.st_mode & S_IROTH){
        file_type[7] = 'r';
    }
    else{
        file_type[7] = '-';
    }
    if(buf.st_mode & S_IWOTH){
        file_type[8] = 'w';
    }
    else{
        file_type[8] = '-';
    }
    if(buf.st_mode & S_IXOTH){
        file_type[9] = 'x';
    }
    else{
        file_type[9] = '-';
    }
    file_type[10] = '\0';

    printf("%s ", file_type);

    //获取文件所有者的用户名组名
    psd = getpwuid(buf.st_uid);
    grp = getgrgid(buf.st_uid);
    printf("%ld ", buf.st_nlink);
    printf("%s\t", psd->pw_name);
    printf("%s\t", grp->gr_name);

    printf("%ld\t", buf.st_size);
    strcpy(buf_time, ctime(&buf.st_mtime));
    buf_time[strlen(buf_time) - 1] = '\0';
    printf("%s ", buf_time);
}


//显示文件信息
void file_information(char *filename){
    int fd;
    char buf[4096];

    fd = open(filename, O_RDONLY);
    if(fd == -1){
        printf("line:%d\n", __LINE__-2);
        perror("open");
    }

    struct stat file_stat;
    stat(filename, &file_stat);

    display_attribute(file_stat);

    printf("%s\n", filename);

    close(fd);
}

//遍历目录
void bianli_dir(char *dirname){
    char filepath[4096];
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);

    while(dir_struct){
        sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
        if(dir_struct->d_type == DT_REG){
            file_information(filepath);
        }
        else if(dir_struct->d_type == DT_DIR){
            if(strcmp(dir_struct->d_name, ".") == 0 || 
               strcmp(dir_struct->d_name, "..") == 0){
                dir_struct = readdir(dir);
                continue;
            }
            file_information(filepath);
            bianli_dir(filepath);
        }
        dir_struct = readdir(dir);
    }
    closedir(dir);
}

void bianli_dir_easy(char *dirname){
    char filepath[4096];
    struct stat buf;
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);

    while(dir_struct){
        if(strcmp(dir_struct->d_name, ".") == 0 || 
             strcmp(dir_struct->d_name, "..") == 0){
            dir_struct = readdir(dir);
            continue;
        }
        stat(dir_struct->d_name, &buf);
        if(S_ISREG(buf.st_mode)){
            if(buf.st_mode & S_IXUSR){
                printf("\033[;32m %s\033[0m\t", dir_struct->d_name);
            }
            else{
                printf("%s\t", dir_struct->d_name);
            }
        }
        else if(S_ISDIR(buf.st_mode)){
            printf("\033[;34m %s\033[0m\t", dir_struct->d_name);
        }
        dir_struct = readdir(dir);
    }
    
    printf("\n");

    closedir(dir);
}

