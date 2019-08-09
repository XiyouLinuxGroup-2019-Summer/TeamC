/*************************************************************************
	> File Name: server_deal.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月08日 星期四 16时18分30秒
 ************************************************************************/
#include "MYSQL.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#define LOGIN 1

typedef struct package{
    int  type;
    int  account;
    char send_name[100];
    int  send_account;
    time_t time;
    char mes[1000];
    char mes2[1000];
} PACK;


int deal_login(PACK pack)
{
    int account = pack.account;
    char password[1000];
    strncpy(password, pack.mes, 1000);
    if(MYSQL_login(account, password)==0) {
        return 0;
    }
}

int deal(PACK pack)
{
    int type = pack.type;
    MYSQL_init();
    switch (type)
    {
        case 1:
            if(deal_login(pack)==0)
            {
                return 0;
            }
            break;
        //case 
    }
}


