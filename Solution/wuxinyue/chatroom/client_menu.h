/*************************************************************************
	> File Name: client_menu.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 23时53分53秒
 ************************************************************************/

#ifndef _CLIENT_MENU_H
#define _CLIENT_MENU_H

typedef enum cli_status {
    INIT = 1,
    LOGIN,
    REGISTER,
    EXIT,
}cli_status_t;

cli_status_t show_init_menu();
cli_status_t show_register_menu();
cli_status_t show_login_menu();

#endif
