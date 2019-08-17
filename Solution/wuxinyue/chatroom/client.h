/*************************************************************************
	> File Name: client.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 21时59分49秒
 ************************************************************************/

#ifndef _CLIENT_H
#define _CLIENT_H

typedef struct client_t {
    int conn_fd;
}client_t;
client_t client;
void cli_show_menu(); 
int client_init(client_t *client);
int client_start(client_t *client);
void client_stop(client_t *client);
void client_destroy(client_t *client);
int user_register(client_t *client, const char *username, const char *password);
int user_login(client_t *client, const char *username, const char *password);
char *user_get_friend_list(client_t *client);
int user_add_friend(client_t *client);
int user_del_friend(client_t *client);
int user_send_message(client_t *client);

#endif
