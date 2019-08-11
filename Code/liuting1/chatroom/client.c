/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月07日 星期三 09时02分25秒
 ************************************************************************/

#include <stdio.h>
#include "client_menu.h"
#include "client_init.h"
#include "client_io.h"
#include <pthread.h>
#include <stdlib.h>

void  main_login(int cli_fd)
{
    char choice;
    choice = login_menu();
    switch (choice){
    case '1':            
        if (send_login_PACK(cli_fd) == 0) {
            main_menu();
        }
        else {
            main_login(cli_fd);
            printf("登录失败，请重新选择登录注册或退出！\n");
        }
        break;
    case '2':
        if (send_regist_PACK(cli_fd) == 0) {
            main_login(cli_fd);
            printf("注册成功,可以登录哟！！！\n");
        }
        else {
            main_login(cli_fd);
            printf("注册失败，请重新注册呀!!\n");
        }
        break;
    case '3':
        if (send_repasswd_PACK(cli_fd) == 0) {
            main_login(cli_fd);
            printf("修改成功,请重新选择登录呀！！\n");
        } 
        else {
            main_login(cli_fd);
            printf("修改失败！！\n");
        }
    default: 
        exit(0); 
    }
    
}

int main()
{
    char choice;
    int cli_fd = init();
    pthread_t recv_tin;
    main_login(cli_fd);
    pthread_create(&recv_tin, NULL, recv_PACK, NULL);
}
