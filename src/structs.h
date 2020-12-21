#pragma once

#include <string.h> 
#include <zmq.h>

#define BUF_SIZE 256
#define MAIN_MODULE -1
#define TO_ALL -42
#define FIRST_WATCHED -87

#define MORE_DATA 1



typedef enum {
    REPLY,
    RECONNECT,
    CHAT,
    ANSWER,
    PING,
    DATA,
    DELETE,
    ERR
} Command;

typedef enum {
    MAIN,
    CALCULATOR
} type_of_node;

typedef struct {
    type_of_node type;
    int MY_ID;
    void* CONTEXT;
    char SSN [BUF_SIZE]; //INPUT_SOCKET_NAME
    void* SS; // SEND_SOCKET
} IDCard ;



typedef struct{
    Command type; 
    int sender;
    int recipient;
    int lastowner;
    char data[BUF_SIZE];
    int moreData;
    int messageID;
} message;

void zmq_message_init( zmq_msg_t* mes, int sender, int recipient, int lastowner, Command command, char* data, int moreData, int messageID  );

void message_init( message* mes, int sender, int recipient, int lastowner, Command command, char* data, int moreData, int messageID  );

void message_standart( zmq_msg_t* mes, int sender ,int recipient, Command command, char* data );