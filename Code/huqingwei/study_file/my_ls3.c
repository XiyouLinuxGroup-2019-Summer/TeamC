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
void bianli_dir_easy_a2(char *dirname);

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
                bianli_dir_easy_a2(".");
            }
            else{
                strcpy(path, argv[2]);
                bianli_dir_easy_a2(path);
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
    //DIR *dir = opendir(dirname);
    DIR *dir;
    //struct dirent *dir_struct = readdir(dir);
    struct dirent *dir_struct;
    char **dirlist;
    int tmp = 0;
    char temp[1024];
    struct stat buf;

    //统计目录下文件的数量
    dir = opendir(dirname);
    if(dir == NULL){
        printf("line: %d", __LINE__-2);
        perror("opendir");
        exit(1);
    }
    while((dir_struct = readdir(dir)) != NULL){
        tmp++;
    }
    closedir(dir);

    //开辟dirlist空间，用来储存目录下的目录文件
    dirlist = (char**)malloc(sizeof(char*)*tmp);
    for(int i=0; i<tmp; i++){
        dirlist[i] = (char*)malloc(sizeof(char)*1024);
    }

    //初始化dirlist
    for(int i=0; i<tmp; i++){
        for(int j=0; j<1024; j++){
            dirlist[i][j] = '\0';
        }
    }

    //将目录属性的文件写入dirlist
    int k = 0;
    dir = opendir(dirname);
    dir_struct = readdir(dir);
    sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
    //printf("tmp = %d\n", tmp);
    for(int i=0; i<tmp; i++){
        //dir_struct = readdir(dir);
        if(dir_struct == NULL){
            printf("line:%d ", __LINE__-2);
            perror("readdir");
        }

        int ret = stat(filepath, &buf);
        if(ret == -1){
            printf("line:%d ", __LINE__-2);
            perror("stat");
            exit(1);
        }
        //printf("a:%s\n", filepath);
        if(S_ISDIR(buf.st_mode)){
            strcpy(dirlist[k], filepath);
            dirlist[k][strlen(filepath)] = '\0';
            //printf("%s\n", dirlist[k]);
            //dir_struct = readdir(dir);
            memset(filepath, 0, 4096);
            sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
            //printf("k = %d\n", k);
            k++;
        }
        else{
            //printf("b\n");
            //dir_struct = readdir(dir);
            memset(filepath, 0, 4096);
            sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
        }
        dir_struct = readdir(dir);
    }
    closedir(dir);
    
    //对dirlist排序
    for(int i=0; i<k; i++){
        for(int j=0; j<k-i-1; j++){
            if(strcmp(dirlist[j], dirlist[j+1]) > 0){
                strcpy(temp, dirlist[j+1]);
                temp[strlen(dirlist[j+1])] = '\0';
                strcpy(dirlist[j+1], dirlist[j]);
                dirlist[j+1][strlen(dirlist[j])] = '\0';
                strcpy(dirlist[j], temp);
                dirlist[j][strlen(temp)] = '\0';
            }
        }
    }

    //printf("k = %d\n", k);

    /*for(int i=0; i<k; i++){
        printf("%s\n", dirlist[i]);
    }
    printf("------------");*/
    
    int i=0;
    while(i < k){
            if(strcmp(dirlist[i], "..") == 0){
                i++;
                continue;
            }
            else{
                printf("%s:\n", dirlist[i]);
                bianli_dir_easy(dirlist[i]);
                //printf("\n");
            }
        i++;
    }
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

//只有ls命令，限时所有文件
void bianli_dir_easy_a2(char *dirname){
    char filepath[4096];
    struct stat buf;
    char **filelist;
    //DIR *dir = opendir(dirname);
    //struct dirent *dir_struct = readdir(dir);
    int tmp;

    stat(dirname, &buf);
    //printf("b\n");
    
    if(S_ISREG(buf.st_mode)){
        printf("a\n");
        if(buf.st_mode & S_IXUSR){
            //file_information(dirname);
            printf("\033[1;32m %s\033[0m\n", dirname);
        }
        else{
            //file_information(dirname);
            printf("%s\n", dirname);
        }
    }
    else{
        tmp = 0;
        //DIR *dir = opendir(dirname);
        DIR *dir;
        //struct dirent *dir_struct = readdir(dir);
        struct dirent *dir_struct;
        char temp[1024];

        //统计目录下文件的数量
        dir = opendir(dirname);
        if(dir == NULL){
            printf("line: %d\n", __LINE__-2);
            perror("opendir");
            exit(1);
        }
        dir_struct = readdir(dir);
        while(dir_struct != NULL){
            tmp++;
            dir_struct = readdir(dir);
        }
        closedir(dir);
        //printf("tmp = %d\n", tmp);

        //malloc空间,字符串数组
        /*for(int i=0; i<tmp; i++){
            filelist[i] = (char*)malloc(sizeof(char)*1024);
        }*/
        //malloc空间,字符串数组
        filelist = (char**)malloc(sizeof(char*)*tmp);
        for(int i=0; i<tmp; i++){
            filelist[i] = (char*)malloc(sizeof(char)*1024);
        }
        
        //printf("a\n");

        //将目录下的文件名写入filelist
        dir = opendir(dirname);
        for(int i=0; i<tmp; i++){
            //printf("a\n");
            dir_struct = readdir(dir);
            if(dir_struct == NULL){
                printf("line: %d\n", __LINE__-2);
                perror("opendir");
            }
            //sprintf(filepath, "%s/%s", dirname, dir_struct->d_name);
            strcpy(filelist[i], dir_struct->d_name);
            filelist[i][strlen(dir_struct->d_name)] = '\0';
            //printf("%s\n", filelist[i]);
        }
        closedir(dir);
        
        //对文件名进行排序
        for(int i=0; i<tmp; i++){
            for(int j=0; j<tmp-i-1; j++){
                if(strcmp(filelist[j], filelist[j+1]) > 0){
                    strcpy(temp, filelist[j+1]);
                    temp[strlen(filelist[j+1])] = '\0';
                    strcpy(filelist[j+1], filelist[j]);
                    filelist[j+1][strlen(filelist[j])] = '\0';
                    strcpy(filelist[j], temp);
                    filelist[j][strlen(temp)] = '\0';
                }
            }
        }
        filelist[tmp] = NULL;
        
        //输出文件
        for(int i=0; i<tmp; i++){
            printf("%-10s  ", filelist[i]);
            if((i+1)%5 == 0){
                printf("\n");
            }
        }
        printf("\n");

        //释放filelist空间
        /*for(int i=0; i<tmp; i++){
            free(*(filelist+i));
        }*/

        /*
        while(dir_struct){
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
        */
    
        //printf("\n");

        //closedir(dir);
    }
}

//只有ls命令，显示当前目录的文件，不包括隐藏文件
void bianli_dir_easy(char *dirname){
    char filepath[4096];
    struct stat buf;
    char **filelist;
    //DIR *dir = opendir(dirname);
    //struct dirent *dir_struct = readdir(dir);
    int tmp;

    stat(dirname, &buf);
    //printf("b\n");
    
    if(S_ISREG(buf.st_mode)){
        printf("a\n");
        if(buf.st_mode & S_IXUSR){
            //file_information(dirname);
            printf("\033[1;32m %s\033[0m\n", dirname);
        }
        else{
            //file_information(dirname);
            printf("%s\n", dirname);
        }
    }
    else{
        tmp = 0;
        //DIR *dir = opendir(dirname);
        DIR *dir;
        //struct dirent *dir_struct = readdir(dir);
        struct dirent *dir_struct;
        char temp[1024];
        
        //统计目录下文件的数量
        //printf("%s\n", dirname);
        dir = opendir(dirname);
        if(dir == NULL){
            printf("line: %d\n", __LINE__-2);
            perror("opendir");
            exit(1);
        }
        dir_struct = readdir(dir);
        while(dir_struct != NULL){
            tmp++;
            dir_struct = readdir(dir);
        }
        closedir(dir);
        //printf("a\n");

        //malloc空间,字符串数组
        /*for(int i=0; i<tmp; i++){
            filelist[i] = (char*)malloc(sizeof(char)*1024);
        }*/
        //malloc空间,字符串数组
        filelist = (char**)malloc(sizeof(char*)*tmp);
        for(int i=0; i<tmp; i++){
            filelist[i] = (char*)malloc(sizeof(char)*1024);
        }
        
        //printf("a\n");

        //将目录下的文件名写入filelist
        dir = opendir(dirname);
        for(int i=0; i<tmp; i++){
            //printf("a\n");
            dir_struct = readdir(dir);
            if(dir_struct == NULL){
                printf("line: %d\n", __LINE__-2);
                perror("opendir");
            }
            strcpy(filelist[i], dir_struct->d_name);
            filelist[i][strlen(dir_struct->d_name)] = '\0';
            //printf("%s\n", filelist[i]);
        }
        closedir(dir);
        
        //对文件名进行排序
        for(int i=0; i<tmp; i++){
            for(int j=0; j<tmp-i-1; j++){
                if(strcmp(filelist[j], filelist[j+1]) > 0){
                    strcpy(temp, filelist[j+1]);
                    temp[strlen(filelist[j+1])] = '\0';
                    strcpy(filelist[j+1], filelist[j]);
                    filelist[j+1][strlen(filelist[j])] = '\0';
                    strcpy(filelist[j], temp);
                    filelist[j][strlen(temp)] = '\0';
                }
            }
        }
        filelist[tmp] = NULL;
        
        //输出文件
        for(int i=0; i<tmp; i++){
            if(filelist[i][0] == '.'){
                continue;
            }
            printf("%-12s", filelist[i]);
            if(i%5 == 0){
                printf("\n");
            }
        }
        printf("\n");

        //释放filelist空间
        /*for(int i=0; i<tmp; i++){
            free(*(filelist+i));
        }*/

        /*
        while(dir_struct){
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
        */
    
        //printf("\n");

        //closedir(dir);
    }
}

//-a，显示当前目录的文件，包括隐藏文件
void bianli_dir_easy_a(char *dirname){
    char filepath[4096];
    struct stat buf;
    //DIR *dir = opendir(dirname);
    //struct dirent *dir_struct = readdir(dir);

    stat(dirname, &buf);
    if(S_ISREG(buf.st_mode)){
        if(buf.st_mode & S_IXUSR){
            printf("\033[1;32m%s\033[0m\n", dirname);
        }
        else{
            printf("%s\n", dirname);
        }
    }
    else{
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
}

//-l命令，显示dirname的信息 
void bianli_dir_l(char *dirname){
    //DIR *dir = opendir(dirname);
    //struct dirent *dir_struct = readdir(dir);
    char filepath[4096];
    struct stat buf;

    stat(dirname, &buf);
    if(S_ISREG(buf.st_mode)){
        if(S_IXUSR & buf.st_mode){
            file_information(dirname);
            printf("\033[1;32m%s\033[0m\n", dirname);
        }
        else{
            file_information(dirname);
            printf("%s\n", dirname);
        }
    }
    
    else{
    DIR *dir = opendir(dirname);
    struct dirent *dir_struct = readdir(dir);
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
