/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月07日 星期三 09时02分25秒
 ************************************************************************/

#include<stdio.h>
#include "client_menu.h"
#include "client_init.h"
#include "client_io.h"



int main()
{
    char choice;
    int cli_fd = init();
    do
    {
        choice = login_menu();
        switch (choice){
            case '1':
                send_login_PACK(cli_fd);
                break;
            case '2':
                //relogin();
                break;

        }
    }while (choice != '0');

    do
    {
        choice = main_menu();

    }while(choice != '0');
}
