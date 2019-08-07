/*************************************************************************
	> File Name: client_menu.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月07日 星期三 11时01分59秒
 ************************************************************************/

#include<stdio.h>
#include <unistd.h>
#include <termios.h>

#include "client_menu.h"

int getch()
{
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return ch;
}


int login_menu()
{
    char choice;
    printf("\n\t\t\33[1m\033[;34m*******************************\033[0m\n");
    printf("\t\t\033[;34m*\033[0m        1.登录                 \033[;34m*\033[0m \n");
    printf("\t\t\033[;34m*\033[0m        2.注册                 \033[;34m*\033[0m \n");
    printf("\t\t\033[;34m*\033[0m        0.退出                 \033[;34m*\033[0m \n");
    printf("\t\t\033[;34m*******************************\033[0m\n");  
    choice = getch();
       
    return choice;
}

int main_menu()
{
    char choice;
    printf("\n\t\t*********************************\n");
    printf("\t\t*        1.好友列表       *\n");
    printf("\t\t*        2.添加好友       *\n");
    printf("\t\t*        3.删除好友       *\n");
    printf("\t\t*        4.群聊信息       *\n");
    printf("\t\t*        5.创建群聊       *\n");
    printf("\t\t*        6.加入群聊       *\n");
    printf("\t\t*        7.退出群聊       *\n");
    printf("\t\t*        8.删除群聊       *\n");
    printf("\t\t*        9.好友聊天       *\n");
    printf("\t\t*        10.群组聊天      *\n");
    printf("\t\t*        11.发送文件      *\n");
    //printf("\t\t*        12.file message box %d  *\n",m_recv_num_file_mes);
    printf("\t\t*        13.聊天记录      *\n");
    printf("\t\t*        0.退出           *\n");
    printf("\t\t************************* *\n");

    return getch();
}

typedef struct{
        char username[MAX_CHAR];
        char password[MAX_CHAR];
    }info_lg;

info_lg *login()
{
   

    Info *info; 
    printf("请输入你的账号：");
    scanf("%s",info->username);
    printf("请输入你的密码：");
    scanf("%s",info->password);

    return info;
}
