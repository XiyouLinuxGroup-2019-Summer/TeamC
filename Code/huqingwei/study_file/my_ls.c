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

void print_filename(char *filename, char *filepath);
void display_attribute(struct stat buf);
void file_information(char *filename);
void bianli_dir(char *dirname);
void bianli_dir_easy(char *dirname);
void bianli_dir_l(char *dirnamr);
void bianli_dir_easy_a(char *dirname);
void bianli_dir_al(char *dirname);
void bianli_dir_R(char *dirname);
void bianli_dir_lR(char *dirname);
void bianli_dir_aR(char *dirname);

int main(int argc, char *argv[])
{
    char path[4096];
    int num = 0;
    int i;

    for(i=0; i<argc; i++){
        if(argv[i][0] == '-')
            num++;
    }

    if(num == 0){
        if(argc == 1){
            bianli_dir_easy(".");
        }
        else{
            strcpy(path, argv[1]);
            bianli_dir_easy(path);
        }
    }
    else{
        char *option = argv[1];

        if(strcmp(option, "-r") == 0){  //-r命令
            if(argc == 2){
                bianli_dir(".");
            }
            else{
                strcpy(path, argv[2]);
                bianli_dir(path);
            }
        }
        else if(strcmp(option, "-l") == 0){ //-l命令
            if(argc == 2){
                bianli_dir_l(".");
            }
            else{
                strcpy(path, argv[2]);
                bianli_dir_l(path);
            }
        }
        else if(strcmp(option, "-a") == 0){
            if(argc == 2){
                bianli_dir_easy_a(".");
            }
            else{
                strcpy(path, argv[2]);
                bianli_dir_easy_a(path);
            }
        }
        else if(strcmp(option, "-al") == 0 || strcmp(option, "-la") == 0){
            if(argc == 2){
                bianli_dir_al(".");
            }
            else{
                strcpy(path, argv[2]);
                bianli_dir_al(path);
            }
        }
        else if(strcmp(option, "-R") == 0){
            if(argc == 2){
                bianli_dir_R(".");
            }
            else{
                strcpy(path, argv[2]);
                bianli_dir_R(path);
            }
        }
        else if(strcmp(option, "-lR") == 0 || strcmp(option, "-Rl") == 0){
            if(argc == 2){
                bianli_dir_lR(".");
            }
            else{
                strcpy(path, argv[2]);
                bianli_dir_lR(path);
            }
        }
        else if(strcmp(option, "-aR") == 0 || strcmp(option, "-lR") == 0){
            if(argc == 2){
                bianli_dir_aR(".");
            }
            else{
                strcpy(path, argv[2]);
                bianli_dir_aR(path);
            }
        }
    }

    return 0;
}

//打印文件名，不同类型不同的显示
void print_filename(char *filename, char *filepath){
    struct stat buf;
    stat(filepath, &buf);

    if(S_ISREG(buf.st_mode)){
        if(buf.st_mode & S_IXUSR){
            printf("\033[1;32m%s\033[0m\n", filename);
        }
        else{
            printf("%s\n", filename);
        }
    }
    else if(S_ISDIR(buf.st_mode)){
        printf("\033[1;34m%s\033[0m\n", filename);
    }
    else if(S_ISLNK(buf.st_mode)){
        printf("\033[1;31m%s\033[0m\n", filename);
    }
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
        file_type[2] = 'w';
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

    printf("%s  ", file_type);

    //获取文件所有者的用户名组名
    psd = getpwuid(buf.st_uid);
    grp = getgrgid(buf.st_uid);
    printf("%ld\t", buf.st_nlink);
    printf("%s\t", psd->pw_name);
    printf("%s\t", grp->gr_name);

    printf("%6ld\t", buf.st_size);
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

    //printf("%s\n", filename);

    close(fd);
}

//-r命令
void bianli_dir(char *dirname){
    char filepath[4096];
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);

    while(dir_struct){
        sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
        if(dir_struct->d_type == DT_REG){
            file_information(filepath);
            print_filename(dir_struct->d_name, filepath);
        }
        else if(dir_struct->d_type == DT_DIR){
            if(strcmp(dir_struct->d_name, ".") == 0 || 
               strcmp(dir_struct->d_name, "..") == 0){
                dir_struct = readdir(dir);
                continue;
            }
            file_information(filepath);
            //printf("%s\n", dir_struct->d_name);
            print_filename(dir_struct->d_name, filepath);
            bianli_dir(filepath);
        }
        dir_struct = readdir(dir);
    }
    closedir(dir);
}

//-R命令
void bianli_dir_R(char *dirname){
    char filepath[4096];
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);

    while(dir_struct){
        //printf("a\n");
        sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
        //printf("%s\n", filepath);
        if(dir_struct->d_type == DT_REG){
            dir_struct = readdir(dir);
            continue;
        }
        else if(dir_struct->d_type == DT_DIR){
            if(strcmp(dir_struct->d_name, "..") == 0){
                dir_struct = readdir(dir);
                continue;
            }
            else if(strcmp(dir_struct->d_name, ".") == 0){
                printf(".:\n");
                bianli_dir_easy(filepath);
            }
            else{
                printf("%s:\n", filepath);
                bianli_dir_easy(filepath);
            }
        }
        dir_struct = readdir(dir);
    }

    closedir(dir);
}

//-lR选项
void bianli_dir_lR(char *dirname){
    char filepath[4096];
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);

    while(dir_struct){
        //printf("a\n");
        sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
        //printf("%s\n", filepath);
        if(dir_struct->d_type == DT_REG){
            dir_struct = readdir(dir);
            continue;
        }
        else if(dir_struct->d_type == DT_DIR){
            if(strcmp(dir_struct->d_name, "..") == 0){
                dir_struct = readdir(dir);
                continue;
            }
            else if(strcmp(dir_struct->d_name, ".") == 0){
                printf(".:\n");
                bianli_dir_l(filepath);
            }
            else{
                printf("%s:\n", filepath);
                bianli_dir_l(filepath);
            }
        }
        dir_struct = readdir(dir);
    }

    closedir(dir);
}

//只有ls命令，显示当前目录的文件，不包括隐藏文件
void bianli_dir_easy(char *dirname){
    char filepath[4096];
    struct stat buf;
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);
    int tmp = 0;

    while(dir_struct){
        if(tmp > 5){
            printf("\n");
        }
        sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
        if(strcmp(dir_struct->d_name, ".") == 0 || 
             strcmp(dir_struct->d_name, "..") == 0){
            dir_struct = readdir(dir);
            continue;
        }
        stat(filepath, &buf);
        if(S_ISREG(buf.st_mode)){
            if(buf.st_mode & S_IXUSR){
                printf("\033[1;32m %s\033[0m\t", dir_struct->d_name);
            }
            else{
                printf("%s\t", dir_struct->d_name);
            }
        }
        else if(S_ISDIR(buf.st_mode)){
            printf("\033[1;34m %s\033[0m\t", dir_struct->d_name);
        }
        dir_struct = readdir(dir);
    }
    
    printf("\n");

    closedir(dir);
}

//-a，显示当前目录的文件，包括隐藏文件
void bianli_dir_easy_a(char *dirname){
    char filepath[4096];
    struct stat buf;
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);

    while(dir_struct){
        sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
        stat(filepath, &buf);
        if(S_ISREG(buf.st_mode)){
            if(buf.st_mode & S_IXUSR){
                //printf("\033[1;32m%s\033[0m\t", dir_struct->d_name);
                printf("\033[1;32m%s\033[0m\t", dir_struct->d_name);
            }
            else{
                printf("%s\t", dir_struct->d_name);
            }
        }
        else if(S_ISDIR(buf.st_mode)){
            printf("\033[1;34m%s\033[m\t", dir_struct->d_name);
        }
        dir_struct = readdir(dir);
    }
    
    printf("\n");

    closedir(dir);
}

//-l命令，显示dirname的信息 
void bianli_dir_l(char *dirname){
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);
    char filepath[4096];

    while(dir_struct){
        sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
        if(strcmp(dir_struct->d_name, ".") == 0 || strcmp(dir_struct->d_name, "..") == 0){
            dir_struct = readdir(dir);
            continue;
        }
        file_information(filepath);
        //printf("%s\n", dir_struct->d_name);
        print_filename(dir_struct->d_name, filepath);
        dir_struct = readdir(dir);
    }

    closedir(dir);
}

//-al命令，显示目录所有文件信息
void bianli_dir_al(char *dirname){
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);
    char filepath[4096];

    while(dir_struct){
        sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);   
        file_information(filepath);
        //printf("%s\n", dir_struct->d_name);
        print_filename(dir_struct->d_name, filepath);
        dir_struct = readdir(dir);
        //if(dir_struct == NULL){
        //    printf("line:%d\n", __LINE__-2);
        //    perror("readdir");
     //   }
        //    
    }

    closedir(dir);
}

//-aR选项
void bianli_dir_aR(char *dirname){
    char filepath[4096];
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);

    while(dir_struct){
        //printf("a\n");
        sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
        //printf("%s\n", filepath);
        if(dir_struct->d_type == DT_REG){
            dir_struct = readdir(dir);
            continue;
        }
        else if(dir_struct->d_type == DT_DIR){
            if(strcmp(dir_struct->d_name, "..") == 0){
                dir_struct = readdir(dir);
                continue;
            }
            else if(strcmp(dir_struct->d_name, ".") == 0){
                printf(".:\n");
                bianli_dir_easy_a(filepath);
            }
            else{
                printf("%s:\n", filepath);
                bianli_dir_easy_a(filepath);
            }
        }
        dir_struct = readdir(dir);
    }

    closedir(dir);
}
