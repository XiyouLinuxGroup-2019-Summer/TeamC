/*************************************************************************
	> File Name: client_menu.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 23时53分45秒
 ************************************************************************/
#include "client_menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#include "socket.h"
#include "client.h"
#include "log.h"
#include "proto.h"

void clear_input_buffer()
{
    while (getchar() != '\n')
    ;
}

int user_register(client_t *client, const char *username, const char *password)
{
    request_register_t *req = create_request_register(username, password);
    // 第一次发包
    int nwritten = tcp_write(client->conn_fd, (char *)req, req->head.length);
    // 判断协议头
    if (nwritten != req->head.length) {
        log_error("send to server failed, exit");
        abort();
    }
    response_status_t *resp = (response_status_t *)malloc(sizeof(response_status_t));
    // 收包 同理
    int nread = tcp_read(client->conn_fd, (char *)resp, sizeof(response_status_t));
    if (nread != sizeof(response_status_t)) {
        log_error("recv from server failed, exit");
        abort();
    }
    if (resp->head.type != RESP_STATUS) {
        log_error("recv from server data type != RESP_STATUS, exit");
        abort();
    }
    if (resp->head.magic != CR_PROTO_HEAD) {
        log_error("recv from server data error, exit");
        abort();
    }
    // 成功resp->status返回0 失败为1
    int status = resp->status;
    free(req);
    free(resp);
    return status;
}
int user_login(client_t *client, const char *username, const char *password)
{
    request_login_t  *req = create_request_login(username, password);
    // 第一次发包
    int nwritten = tcp_write(client->conn_fd, (char *)req, req->head.length);
    // 判断协议头
    if (nwritten != req->head.length) {
        log_error("send to server failed, exit");
        abort();
    }
    response_status_t *resp = (response_status_t *)malloc(sizeof(response_status_t));
    // 收包 同理
    int nread = tcp_read(client->conn_fd, (char *)resp, sizeof(response_status_t));
    if (nread != sizeof(response_status_t)) {
        log_error("recv from server failed, exit");
        abort();
    }
    if (resp->head.type != RESP_STATUS) {
        log_error("recv from server data type != RESP_STATUS, exit");
        abort();
    }
    if (resp->head.magic != CR_PROTO_HEAD) {
        log_error("recv from server data error, exit");
        abort();
    }
    // 成功resp->status返回0 失败为1
    int status = resp->status;
    free(req);
    free(resp);
    return status;
}


cli_status_t show_register_menu()
{
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    printf("请输入您的用户名:");
    scanf("%s", username);
    printf("请输入您的密码:");
    scanf("%s", password);
    int status = user_register(&client, username, password);
    if (status == 0) {
        printf("注册成功\n");
    }
    else {
        printf("注册失败\n");
    }
    return INIT;
}

cli_status_t show_login_menu()
{
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    printf("请输入您的用户名:");
    scanf("%s", username);
    printf("请输入您的密码:");
    scanf("%s", password);
    int status = user_login(&client, username, password);
    if (status == 0) {
        printf("登录成功");
    } else {
        printf("登录失败");
    }
    return INIT;
}

cli_status_t show_init_menu()
{
    time_t Time = time(NULL);
	char *buf = ctime(&Time);
	printf("%s", buf);
    int  choice = 0;
	printf("|****************************|\n");
	printf("|          欢迎使用          |\n");
	printf("|****************************|\n");
	printf("|       1、用户注册          |\n");
	printf("|       2、用户登录          |\n");
	printf("|       0、退出              |\n");
    printf("|****************************|\n");
	printf("请输入您要进行的操作:\n");
    scanf("%d", &choice);
    switch (choice) {
        case 1:
        return REGISTER;
        case 2:
        return LOGIN;
        case 0:
        return EXIT;
        default:
        printf("输入错误!\n");
        break;
    }
    return INIT;
}

void cli_show_menu()
{
    enum cli_status status = INIT;
    while (status != EXIT) {
        switch(status) {
            case INIT:
                status = show_init_menu();
                break;
            case REGISTER:
                status = show_register_menu();
                break;
            case LOGIN:
                status = show_login_menu();
            case EXIT:
                break;
            default:
                log_error("status unknown: %d", status);
                abort();
        }
        clear_input_buffer();
    }
}



