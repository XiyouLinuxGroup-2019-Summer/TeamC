/************************************************************************
	> File Name: server_deal.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月08日 星期四 16时18分30秒
 ************************************************************************/
#include "MYSQL.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


static int cli_fd;

void send_PACK(PACK pack);
void recv_PACK(int conn_fd);
void deal_regist(PACK pack);
void deal_login(PACK pack);
void deal_repass(PACK pack);
void deal_exit(PACK pack);
void deal_addfriend(PACK pack);
void deal_list_fri(PACK pack);
void deal_online_fri(PACK pack);
void deal_chat_fri(PACK pack);
void deal_store_fri(PACK);
void deal_dele_fri(PACK);
void deal_creat_group(PACK);
void deal_join_group(PACK);
void deal_quit_group(PACK);
void deal_dele_group(PACK);
void deal_group_mes(PACK);
void deal_chat_group(PACK);
void deal_find_store_g(PACK);
void deal_set_up(PACK);
void deal_join_user(PACK);
void deal_dele_user(PACK);

void deal(PACK pack);
void pro_off(int , int);
void deal_fri_off(PACK pack);
void deal_group_off(PACK pack);
void recv_addfr_PACK(PACK);
void send_file(PACK);

void recv_PACK(int conn_fd)
{
    PACK pack;
    cli_fd = conn_fd;
    int ret;
    char *str = (char *)&pack;
    int sum = 0;
    while((ret = recv(cli_fd, str + sum,sizeof(PACK) - sum ,0)) > 0) {
        sum += ret;
        if (sum == sizeof(PACK)) {
            break;
        }
    }
    bzero(&pack,sizeof(PACK));
    if((ret = recv(conn_fd, &pack, sizeof(struct package),0)) < 0){
        perror("recv");
        exit(1);
    }
    if (ret == 0)
    {
        return ;
    }
    deal(pack);
}

void send_PACK(PACK pack)
{
    int ret;
    if((ret =  send(cli_fd,&pack,sizeof(PACK),0))<0) {
        perror("server_deal ：send");
    }
}

void send_other_PACK(PACK pack,int send_cli_fd)
{
    int ret;
    if((ret =  send(send_cli_fd,&pack,sizeof(PACK),0))<0) {
        perror("server_deal ：send");
    }
}


void deal(PACK pack)
{
    int type = pack.type;
    MYSQL_init();
    pthread_t send_f;
    pthread_t send2_f;
    switch (type)
    {
        case 0:
            deal_exit(pack);
            break;
        case 1:
            deal_login(pack);
            break;
        case 2:
            deal_regist(pack);
            break;
        case 3:
            deal_repass(pack);
            break;
        case 4:
            deal_addfriend(pack);
            break;
        case 41:
            recv_addfr_PACK(pack);
            break;
        case 5:
            deal_list_fri(pack);
            break;
        case 6:
            deal_online_fri(pack);
            break;
        case 7:
            deal_chat_fri(pack);
            break;
        case 8:
            deal_store_fri(pack);
            break;
        case 9:
            deal_dele_fri(pack);
            break;
        case 10:
            deal_creat_group(pack);
            break;
        case 11:
            deal_join_group(pack);
            break;
        case 12:
            deal_quit_group(pack);
            break;
        case 13:
            deal_dele_group(pack);
            break;
        case 14:
            deal_group_mes(pack);
            break;
        case 15:
            deal_chat_group(pack);
            break;
        case 16:
            deal_find_store_g(pack);
            break;
        case 17:
            deal_set_up(pack);
            break;
        case 18:
            send_file(pack);
            break; 
        case JOIN_USER:
            deal_join_user(pack);
            break;
        case DELE_USER:
            deal_dele_user(pack);
            break;
    }
}
void deal_login(PACK pack)
{
    PACK send_pack;
   
    int ret = MYSQL_login(pack.account, pack.mes,cli_fd);
    if(ret == 0) {
        send_pack.type = 0;
        strcpy(send_pack.mes,"登录成功");
    }
    else {
        send_pack.type = -1;
    }
    send_PACK(send_pack);
    pro_off(pack.account, cli_fd);        

}

void deal_regist(PACK pack)
{
    PACK send_pack;

    int ret = MYSQL_regist(pack.mes, pack.mes2);
    if(ret != -1) {
        send_pack.type = 0;
        send_pack.account = ret;
        strcpy(send_pack.mes,"注册成功");

    }
    else {
        send_pack.type = -1;
    }
    send_PACK(send_pack);
}

void deal_repass(PACK pack)
{
    PACK send_pack;

    int ret = MYSQL_repass(pack.account, pack.mes, pack.mes2);
    if(ret != -1) {
        send_pack.type = 0;
        strcpy(send_pack.mes,"重置密码成功");
    }
    else {
        send_pack.type = -1;
    }
    send_PACK(send_pack);
}

void deal_exit(PACK pack)
{
    PACK send_pack;
    int ret   = MYSQL_exit(pack.account);
    if(ret != -1) {
        send_pack.type = 0;
    }
    else {
        send_pack.type =-1;
    }
    printf("客户端%d退出成功\n",pack.account);
    send_PACK(send_pack);

}

void deal_list_fri(PACK pack)
{
    PACK send_pack;
    fri p = MYSQL_list_fri(pack.account);
    if(send(cli_fd, &pack, sizeof(PACK), 0)<0) {
        perror("deal_online_fri");
    }
    if((send(cli_fd,&p,sizeof(fri),0))<0) {
        perror("server_deal ：send");
    }

}

void deal_online_fri(PACK pack)
{
    fri p = MYSQL_online_fri(pack.account);
    if(send(cli_fd, &pack, sizeof(PACK), 0)<0) {
        perror("deal_online_fri");
    }
    if((send(cli_fd,&p,sizeof(fri),0))<0) {
        perror("server_deal ：send");
    }

}

void deal_chat_fri(PACK pack)
{
    int ret = MYSQL_find_frirela(pack.account, pack.send_account);
    if(ret == -1) {
        PACK send_pack;
        send_pack.type = CHAT_FRI;
        send_pack.account = pack.account;
        strcpy(send_pack.mes, "输入账号不存在，或者不是好友");
        send(cli_fd, &send_pack, sizeof(PACK), 0);
        return ;
    }
    fri f = MYSQL_find_fd(pack.send_account);
    fri f2 = MYSQL_find_fd(pack.account);
    strcpy(pack.send_name , f2.name[0]);
    if(MYSQL_store_chat(pack.account,pack.send_account,pack.mes) != 0) {
         printf("聊天记录存入失败\n");
    }
    if(f.online[0] == 0) {
        deal_fri_off(pack);
    }
    else {
        send_other_PACK(pack, f.account[0]);
    }

}

void deal_store_fri(PACK pack)
{
    PACK send_pack;
    STR str = MYSQL_find_chat(pack.account, pack.send_account);
    if(str.account[0] == -1)
    {
       strcmp(send_pack.mes, "没有聊天记录或者查询失败");
    }
    send_pack.type = STORE_CHAT;
    if(send(cli_fd, &send_pack, sizeof(PACK), 0)<0) {
        perror("deal_store_fri");
    }
    int ret;
    if((ret=send(cli_fd, &str, sizeof(STR),0)) <=0 ) {
        perror("send_store_fri");
    }
}
void deal_dele_fri(PACK pack)
{
    MYSQL_dele_fri(pack.account, pack.send_account);
}

void deal_creat_group(PACK pack)
{
    PACK send_pack;
    int ret = MYSQL_creat_group(pack.account, pack.mes);
    if(ret != -1) {
        send_pack.type = CREAT_GROUP;
        send_pack.account = ret;
        strcpy(send_pack.mes,"创群成功");
    }
    else {
        strcpy(send_pack.mes,"创群失败");
    }
    send_PACK(send_pack);

}

void deal_join_group(PACK pack)
{
    PACK send_pack;
    send_pack.type = JOIN_GROUP;
    int ret = MYSQL_find_group(pack.send_account);
    if(ret == 0)
    {
        int ret = MYSQL_find_group_member(pack.account, pack.send_account);
        if (ret == 0) {
            strcpy(send_pack.mes,"你已是该群成员");
        } 
        else {
            ret = MYSQL_join_group(pack.account, pack.send_account);
            if (ret == 0)
            {
                strcpy(send_pack.mes, "加入群聊成功,欢迎欢迎！！！");
            }
            else {
                strcpy(send_pack.mes, "写入数据库出错，请重新加入");
            }
        }
    }
    else {
        strcpy(send_pack.mes, "该群不存在");
    }
    send_PACK(send_pack);
}

void deal_quit_group(PACK pack)
{
    PACK send_pack;
    int ret = MYSQL_find_group_member(pack.account, pack.send_account);
    send_pack.type = QUIT_GROUP;
    if (ret == -1) {
        strcpy(send_pack.mes,"该群号不存在或你并不是该群成员");
    } 
    else {
        ret = MYSQL_quit_group(pack.account, pack.send_account);
        if (ret == 0)
        {
            strcpy(send_pack.mes, "退出群聊成功");
        }
        else {
            strcpy(send_pack.mes, "写入数据库出错，请重新退出");
        }
    }
    send_PACK(send_pack);
}

void deal_dele_group(PACK pack)
{
    PACK send_pack;
    int ret = MYSQL_find_gowner(pack.account, pack.send_account);
    send_pack.type = DELE_GROUP;
    if (ret == -1) {
        strcpy(send_pack.mes,"该群号不存在或者你并不是群主，没有权限！！！");
    } 
    else {
        ret = MYSQL_dele_group(pack.send_account);
        if (ret == 0)
        {
            strcpy(send_pack.mes, "删除群聊成功");
        }
        else {
            strcpy(send_pack.mes, "写入数据库出错，请重新删除");
        }
    }
    send_PACK(send_pack);
}

void deal_group_mes(PACK pack)
{
    PACK send_pack;
    GROUP group;
    send_pack.type = GROUP_MES;
    char *p =  MYSQL_group_name(pack.send_account);
    int tmp;
    tmp = strcmp(p, "fail");
    if(tmp) {
        strcpy(send_pack.mes,p);
        group = MYSQL_group_mes(pack.send_account);
        group = MYSQL_find_user(group);
    }
    else {
        strcpy(send_pack.mes, "查询失败或该群不存在");
    }
    send_PACK(send_pack);
    int ret;
    if((ret=send(cli_fd, &group, sizeof(GROUP),0)) <=0 ) {
        perror("send_store_fri");
    }
    
}

void deal_chat_group(PACK pack) 
{
    int ret = MYSQL_find_group_member(pack.account, pack.send_account);
    if (ret == -1) {
        strcpy(pack.mes,"该群号不存在或你并不是该群成员");
        send_PACK(pack);
        return;
    }
    ret = MYSQL_store_group(pack.account, pack.send_account, pack.mes);
    if(ret == -1) {
        printf("聊天信息存入失败");
    }
    PACK send_pack;
    GROUP group;
    send_pack.type = CHAT_GROUP;
    send_pack.account =  pack.send_account;  //群聊账号
    strcpy(send_pack.mes2, pack.mes);
    char *p =  MYSQL_group_name(pack.send_account);
    int tmp;
    tmp = strcmp(p, "fail");
    if(tmp) {
        strcpy(send_pack.send_name, p); //群聊名称
        group = MYSQL_group_mes(pack.send_account);  //发送人名称
        group = MYSQL_find_user(group);
        for(int i= 0; i < group.len; i++) {
            if(group.cli_fd[i] == cli_fd) {
                strcpy(send_pack.mes, group.name[i]);
            }
        }
        for(int i= 0; i < group.len; i++) {
            send_pack.send_account = group.account[i]; 
            
            if(group.cli_fd[i] != cli_fd) {
                if(group.online[i] == 1) {
                    send_other_PACK(send_pack, group.cli_fd[i]);
                }
                else {
                    deal_group_off(send_pack); 
                }
            }

        }
    }
    else {
        strcpy(send_pack.mes, "查询失败或该群不存在");
        send_PACK(send_pack);

    }
   
}

void deal_find_store_g(PACK pack) 
{
    char *p =  MYSQL_group_name(pack.send_account);
    int tmp;
    tmp = strcmp(p, "fail");
    if(tmp) {
        strcpy(pack.send_name, p); //群聊名称
    }
    else {
        strcpy(pack.mes, "查询失败或该群不存在");
        send_PACK(pack);
        return;
    }

    STR_G group = MYSQL_find_chat_group(pack.send_account);
    if(group.usr_account[0] == -1)
    {
       strcmp(pack.mes, "没有聊天记录或者查询失败");
       send_PACK(pack);
       return;
    }
    strcpy(pack.mes ,"success");
    send_PACK(pack);
    int ret;
    if((ret=send(cli_fd, &group, sizeof(STR_G),0)) <=0 ) {
        perror("send_store_fri");
    }
    
    return;
}


void deal_set_up(PACK pack)
{
    int set_account;
    int ret = MYSQL_find_gowner(pack.account, pack.send_account);
    if(ret == 0) {
        set_account =atoi(pack.mes);
        ret = MYSQL_find_group_member(set_account, pack.send_account);
        if(ret == 0) {
            MYSQL_set_up(pack.send_account, set_account);
            strcpy(pack.mes, "设置成功,他已经是管理员了哦！！");
        }
        else {
            strcpy(pack.mes, "你要设置的并不是群成员，快去拉他进群吧!!!");
        }
    }
    else {
        pack.send_account = 0;
        strcpy(pack.mes,"该群不存在，或者你并不是群主，无法操作");
    }
    send_PACK(pack);
    
}

void deal_join_user(PACK pack)
{
    int ret;
    int set_account;
    ret = MYSQL_find_group_member(pack.account, pack.send_account);
    if(ret == 0) {
        set_account = atoi(pack.mes);
        ret = MYSQL_find_group_member(set_account, pack.send_account);
        if(ret == 0)
        {
            strcpy(pack.mes, "他已经是该群成员，快去聊天吧！！");
            send_PACK(pack);
            return;
        }
        MYSQL_join_group(set_account , pack.send_account);
        fri f = MYSQL_find_fd(pack.send_account);
        strcpy(pack.mes, "你已成功邀请他进群");
        send_PACK(pack);
        sprintf(pack.mes, "你被账号%d拉进群%d",pack.account, pack.send_account);
        pack.send_account = set_account;
        if(f.online[0] == 1) {
            send_other_PACK(pack,f.account[0]); 
        }
        else {
            deal_fri_off(pack);
        } 
    }
    else {
        strcpy(pack.mes, "该群不存在，或者你并不是该群成员，无法操作!!!");
        send_PACK(pack);
    }
    
}

void deal_dele_user(PACK pack)
{
    int ret;
    int set_account;
    ret = MYSQL_find_group_vip(pack.account, pack.send_account);
    if(ret == 0) {
        set_account = atoi(pack.mes);
        MYSQL_quit_group(set_account , pack.send_account);
        fri f = MYSQL_find_fd(pack.send_account);
        strcpy(pack.mes, "你已成功从群聊中删除他");
        send_PACK(pack);
        pack.send_account = set_account;
        sprintf(pack.mes, "你被账号%d从群%d删除",pack.account, pack.send_account);
         if(f.online[0] == 1) {
            send_other_PACK(pack,f.account[0]); 
        }
        else {
            deal_fri_off(pack);
        }       
    }
    else {
        strcpy(pack.mes, "该群不存在，或者你并不是该群管理员，无法操作!!!");
        send_PACK(pack);
    }
}

typedef struct off{
    char mes[1000];
    int  account;
    char send_name[100];
    int  send_account;
    int  type;
    char mes2[1000];
    struct off *next;
}OFF;
OFF *phead = NULL;
OFF *pend = NULL;

void deal_group_off(PACK pack)
{
    if (phead == NULL) {
        phead = (OFF *)malloc(sizeof(OFF));
        phead->next =NULL;        
        pend = phead;
    }
    OFF *pnew;
    pnew = (OFF *)malloc(sizeof(OFF));
    strcpy(pnew->mes , pack.mes);
    strcpy(pnew->mes2,pack.mes2);
    pnew->account = pack.account;
    strcpy(pnew->send_name , pack.send_name);
    pnew->send_account = pack.send_account;
    pnew->type = pack.type;
    pnew->next = NULL;
    pend->next = pnew;
    pend = pnew;
}
void deal_fri_off(PACK pack)
{
    if (phead == NULL) {
        phead = (OFF *)malloc(sizeof(OFF));
        phead->next =NULL;        
        pend = phead;
    }
    OFF *pnew;
    pnew = (OFF *)malloc(sizeof(OFF));
    strcpy(pnew->mes , pack.mes);
    //strcpy(pnew->mes2,pack.mes2);
    pnew->account = pack.account;
    //strcpy(pnew->send_name , pack.send_name);
    pnew->send_account = pack.send_account;
    pnew->type = pack.type;
    pnew->next = NULL;
    pend->next = pnew;
    pend = pnew;
}
void pro_off(int send_account, int send_cli_fd)
{
    if (phead == NULL) {
        return;
    }
    OFF *r = phead;
    OFF * p = NULL;
    while (r->next) {
        p = r->next;
        if (p->send_account == send_account) {
            PACK send_pack;
            strcpy(send_pack.mes , p->mes);
            send_pack.type = p->type;
            send_pack.account = p->account;
            send_pack.send_account = p->send_account;
            strcpy(send_pack.mes2, p->mes2);
            strcpy(send_pack.send_name, p->send_name);
            send_other_PACK(send_pack,send_cli_fd);
            PACK pack;
            r->next = p->next;
            free(p);
            continue;
        }
        r = r->next;
    }
    pend = r;
}

void deal_addfriend(PACK pack)
{
    int ret = MYSQL_find_frirela(pack.account, pack.send_account);
    if(ret == 0) {
        strcpy(pack.mes,"你们已经是好友喽，直接去聊天吧");
        send(cli_fd, &pack, sizeof(PACK), 0);
        return;
    }
    PACK send_pack;
    fri f = MYSQL_find_fd(pack.send_account);
    fri f2= MYSQL_find_fd(pack.account);
    if(f.account[0] == -1) {
        strcpy(pack.mes,"你输入的账号不正确，请正确输入!!!");
        send(cli_fd, &pack, sizeof(PACK), 0);
        return ;
    }
    strcpy(pack.send_name , f2.name[0]);
    sprintf(send_pack.mes,"账号%s申请加你为好友，是否同意",pack.send_name); 
    send_pack.account = pack.account;
    send_pack.send_account = pack.send_account;
    send_pack.type = ADD_FRIEND;
    if(f.online[0] == 0) {
        deal_fri_off(send_pack);
    }
    else {
        send_other_PACK(send_pack,f.account[0]);      
    }
    
}

void recv_addfr_PACK(PACK recv_pack)
{
    if(recv_pack.type == 41) {
            fri fri = MYSQL_find_fd(recv_pack.send_account);
            if(strcmp(recv_pack.mes,"success") == 0) {
                sprintf(recv_pack.mes,"你和%d已经成功加为好友了，快去聊天吧！！",recv_pack.account);
                if(MYSQL_addfriend_store(recv_pack.account , recv_pack.send_account) == 0) {
                    printf("添加好友信息成功!!!\n");
                }     
            }
            else {
                    sprintf(recv_pack.mes,"%d拒绝了你的好友申请，他真是个傻逼！！",recv_pack.account);

                }
            send_other_PACK(recv_pack, fri.account[0]);
    }
    else {
            printf("好友信息返回失败\n");
    }
}


int file_send_account = 0;
fri f;
void send_file(PACK pack)
{
    if(file_send_account == 0) {
        printf("#######\n");
	    file_send_account = pack.send_account;
        f = MYSQL_find_fd(pack.send_account);
        send_other_PACK(pack, f.account[0]);

    }
    else {
         if(file_send_account == pack.send_account) {
	    send_other_PACK(pack, f.account[0]);
	 }
	 else {
         printf("&&&&&&&&&\n");
	        file_send_account = pack.send_account;
            f = MYSQL_find_fd(pack.send_account);
            send_other_PACK(pack, f.account[0]);
        }
    }
}

/*void *SEND_File(void *arg)
{
    pthread_detach(pthread_self());
    PACK pack = *(PACK *)arg;
    PACK send_pack;
    int fd;
    if( (fd = open("/home/lt/test/test.c", O_RDWR|O_CREAT|O_APPEND, S_IRUSR| S_IWUSR)) == -1) {
        perror("open");
    }

    if(write(fd, pack.mes, 999) < 0) {
        perror("write");
    }

    send_pack.type = SEND_FILE;
    send_PACK(send_pack);
    close(fd);

}*/

/*void *send2_file(void *arg)
{
    pthread_detach(pthread_self());
    PACK send_pack;
    memset(&send_pack, 0, sizeof(send_pack));
    
    int fd;
    char path[100];
    strcpy(path, "home/lt/tmp");
    if((fd = open(path, O_RDONLY)) == -1) {
            perror("**send_open");
    }

    while(read(fd, send_pack.mes, 999) > 0) {
        send_pack.type = OK_FILE;
        send_PACK(send_pack);
        memset(&send_pack, 0, sizeof(send_pack));
    }

    close(fd);

}*/
