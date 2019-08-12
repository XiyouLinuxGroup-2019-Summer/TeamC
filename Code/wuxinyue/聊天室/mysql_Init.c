/*************************************************************************
	> File Name: mysql_Init.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月09日 星期五 16时55分32秒
 ************************************************************************/
#include <stdio.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>
#include "mysql_Init.h"

typedef struct msg {
    char username[20];
    char password[20];
    int  type;
    char filename[20];
    char time[10];
}Msg;

Msg msg;

int                 i;
int                 ret;
unsigned int        num_fields;
MYSQL               mysql;
MYSQL_RES           *result = NULL;
MYSQL_ROW           row;
MYSQL_FIELD         *field;

void  accept_mysql()
{	
	if(NULL == mysql_init(&mysql)){
		perror("mysql_init\n");
	}

	// 初始化数据库
	if(mysql_library_init(0, NULL, NULL) != 0){
		perror("mysql_library_init\n");
	}

	// 连接数据库
	if(NULL == mysql_real_connect(&mysql, "127.0.0.1", "root", "102089wy", "care", 0, NULL, 0)){
		perror("mysql_real_connect\n");
	}

	// 设置中文字符集
	if(mysql_set_character_set(&mysql, "utf8") < 0){
		perror("mysql_set_character_set\n");
	}
	
	printf("连接mysql数据库成功\n");
}
void  Init_mysql() {
    accept_mysql();
    if(mysql_query(&mysql, "update user set online = 1" )){
        perror("Init_mysql:mysql_query\n");
    }
}
void  MYSQL_regis() {
    int flag = 0, max = 4;
    mysql_query(&mysql, "select *from user");
    result = mysql_store_result(&mysql);
    if (result){
        // num_fields代表这张表的列数
        // num_fields = mysql_num_fields(result);
        // 这个函数会返回一行
        while((row = mysql_fetch_row(result))){
            if (strcmp(row[1], msg.username) == 0){
                msg.type = -1;
                flag = 1;
                break;
            }
        }
    }
    if (flag == 0) {
        char buff[1000];
        printf("输入成功\n");
        max++;
        sprintf(buff, "insert into user(username, password,ID) values ('%s', '%s', %d)", msg.username, msg.password, max);
        int ret = mysql_query(&mysql, buff);
        if (ret){
            perror("MYSQL_regis: mysql_query\n");
        }
        msg.type = 1001;
    }
    mysql_close(&mysql);
}

void MYSQL_login() {
    int flag = 1;
    mysql_query(&mysql, "select *from user");
    result = mysql_store_result(&mysql);
    if (result) {
        // num_fields = mysql_num_fields(result);
        while((row = mysql_fetch_row(result))) {
            if ((strcmp(row[1], msg.username) == 0) && (strcmp(row[2], msg.password) == 0)) {
                msg.type = 1002;
                flag = 0;
                break;
            }
        }
    }
    if (flag == 1) {
        msg.type = -2;
    }
}
