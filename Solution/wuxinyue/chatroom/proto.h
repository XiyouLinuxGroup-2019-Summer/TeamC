/*************************************************************************
	> File Name: proto.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 18时40分50秒
 ************************************************************************/

#ifndef _PROTO_H
#define _PROTO_H

#define CR_PROTO_HEAD 0xAAAAA

#include<stdint.h>

#define PROTR_TYPE_BASE 1000

typedef enum proto_type_t {
    REQ_REGISTER = 1000,
    REQ_LOGIN = 1001,
    REQ_GET_FRIEND_LIST = 1002,
    REQ_ADD_FRIEND = 1003,
    REQ_DEL_FRIEND = 1004,
    REQ_SEND_MESSAGE = 1005,

    RESP_STATUS = 2001,
    RESP_FRIEND_LIST = 2002,
    RESP_MESSAGE = 2003,
} proto_type_t;

#define USERNAME_LEN 32
#define PASSWORD_LEN 32

typedef struct proto_head_t {
    uint32_t magic;
    uint32_t type;
    uint32_t length;
} proto_head_t __attribute__((aligned(1)));

// request
typedef struct request_register_t {
    proto_head_t head;
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
} request_register_t __attribute__((aligned(1)));

typedef struct request_login_t {
    proto_head_t head;
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
} request_login_t __attribute__((aligned(1)));

typedef struct request_get_friend_list_t {
    proto_head_t head;
} request_get_friend_list_t __attribute__((aligned(1)));

typedef struct request_add_friend_t {
    proto_head_t head;
    char friendname[PASSWORD_LEN];
} request_add_friend_t __attribute__((aligned(1)));

typedef struct request_del_friend_t {
    proto_head_t head;
    char friendname[PASSWORD_LEN];
} request_del_friend_t __attribute__((aligned(1)));

typedef enum message_type_t {
    SINGLE = 0,
    GROUP = 1,
} message_type_t;

typedef struct request_send_message_t {
    proto_head_t head;
    message_type_t type;
    char target_name[USERNAME_LEN];
    char messgae[0];
} request_send_message_t __attribute__((aligned(1)));

// response
#define MSG_LEN 32
typedef struct response_status_t {
    proto_head_t head;
    int status;
    char message[MSG_LEN];
} response_status_t __attribute__((aligned(1)));

#define USERLIST_LEN 320
typedef struct response_friens_list_t {
    proto_head_t head;
    char userlist[USERLIST_LEN];
} response_friens_list_t __attribute__((aligned(1)));

typedef struct response_message_t {
    proto_head_t head;
    message_type_t type;
    char target_name[USERNAME_LEN];
    uint16_t message_Len;
    char messgae[0];
} response_message_t __attribute__((aligned(1)));

request_register_t *create_request_register(const char *username, const char *password);
request_login_t *create_request_login();
request_get_friend_list_t *create_request_get_friend_list();
request_add_friend_t *create_request_add_friend();
request_del_friend_t *create_request_del_friend();
request_send_message_t *create_request_send_message(int len);

response_status_t *create_response_status(int status, const char *msg);
response_friens_list_t *create_response_friends_list();
response_message_t *create_response_message(int len);


#endif // PROTO_H

