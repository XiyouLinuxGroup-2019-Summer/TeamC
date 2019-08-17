/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 22时01分50秒
 ************************************************************************/

#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "define.h"
#include "socket.h"
#include "proto.h"
#include "log.h"

#define DEFAULT_ADDR "127.0.0.1"

 client_t client;
int client_init(client_t *client)
{
    client->conn_fd = 0;

    return CR_OK;
}

int client_start(client_t *client)
{
    client->conn_fd = tcp_connect(DEFAULT_ADDR, LISTEN_PORT);
    if (client->conn_fd < 0) {
        log_warn("client conn %s:%d failed", DEFAULT_ADDR, LISTEN_PORT);
        abort();
    }

    return CR_OK;
}

void client_destroy(client_t *client)
{
    log_warn("client conn %s:%d closed", DEFAULT_ADDR, LISTEN_PORT);
    tcp_close(client->conn_fd);
}


int main(int argc, char *argv[])
{
    set_log_level(LOG_DEBUG);

    client_init(&client);
    client_start(&client);

    cli_show_menu();

    client_destroy(&client);

    return EXIT_SUCCESS;
}

