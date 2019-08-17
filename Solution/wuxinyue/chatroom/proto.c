/*************************************************************************
	> File Name: proto.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 19时12分19秒
 ************************************************************************/

#include "proto.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

static void init_packet_head(proto_head_t *head, proto_type_t type, int length)
{
    head->magic = CR_PROTO_HEAD;
    head->type = type;
    head->length = length;
}

request_register_t *create_request_register(const char *username, const char *password)
{
    request_register_t *packet = (request_register_t *)malloc(sizeof(request_register_t));
    init_packet_head(&packet->head, REQ_REGISTER, sizeof(request_register_t));
    strncpy(packet->username, username, USERNAME_LEN);
    strncpy(packet->password, password, PASSWORD_LEN);
    return packet;
}

request_login_t *create_request_login()
{
    request_login_t *packet = (request_login_t *)malloc(sizeof(request_login_t));
    init_packet_head(&packet->head, REQ_LOGIN, sizeof(request_login_t));
    return packet;
}

request_get_friend_list_t *create_request_get_friend_list()
{
    request_get_friend_list_t *packet = (request_get_friend_list_t *)malloc(sizeof(request_get_friend_list_t));
    init_packet_head(&packet->head, REQ_GET_FRIEND_LIST, sizeof(request_get_friend_list_t));
    return packet;
}

request_add_friend_t *create_request_add_friend()
{
    request_add_friend_t *packet = (request_add_friend_t *)malloc(sizeof(request_add_friend_t));
    init_packet_head(&packet->head, REQ_ADD_FRIEND, sizeof(request_add_friend_t));
    return packet;
}

request_del_friend_t *create_request_del_friend()
{
    request_del_friend_t *packet = (request_del_friend_t *)malloc(sizeof(request_del_friend_t));
    init_packet_head(&packet->head, REQ_DEL_FRIEND, sizeof(request_del_friend_t));
    return packet;
}

request_send_message_t *create_request_send_message(int len)
{
    int length = sizeof(request_send_message_t) + len;
    request_send_message_t *packet = (request_send_message_t *)malloc(length);
    init_packet_head(&packet->head, REQ_SEND_MESSAGE, length);
    return packet;
}

response_status_t *create_response_status(int status, const char *msg)
{
    response_status_t *packet = (response_status_t *)malloc(sizeof(response_status_t));
    init_packet_head(&packet->head, RESP_STATUS, sizeof(response_status_t));
    packet->status = status;
    strncpy(packet->message, msg, MSG_LEN);
    
    return packet;
}

response_friens_list_t *create_response_friends_list()
{
    response_friens_list_t *packet = (response_friens_list_t *)malloc(sizeof(response_friens_list_t));
    init_packet_head(&packet->head, RESP_FRIEND_LIST, sizeof(response_status_t));
    return packet;
}

response_message_t *create_response_message(int len)
{
    int length = sizeof(request_send_message_t) + len;
    response_message_t *packet = (response_message_t *)malloc(length);
    init_packet_head(&packet->head, RESP_MESSAGE, length);
    return packet;
}



