/*************************************************************************
	> File Name: mysql.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月07日 星期三 09时03分15秒
 ************************************************************************/

#ifndef _MYSQL_H
#define _MYSQL_H
void MYSQL_main_init();
void MYSQL_init();
int MYSQL_login(int, char *, int);
int MYSQL_regist(char *, char *);
int MYSQL_repass(int ,char *, char *);
#endif
