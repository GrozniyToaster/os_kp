#pragma once

#include <string.h> 
#include <zmq.h>
#include <stdbool.h>

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
    TURN,
    ERROR,
    OPPONENT_WIN
} Command;

typedef enum {
    MAIN,
    CALCULATOR
} type_of_node;

typedef struct {
    type_of_node type;
    int MY_ID;
    void* CONTEXT;
    char SSN[BUF_SIZE]; //INPUT_SOCKET_NAME
    void* SS; //SEND_SOCKET
} IDCard;

typedef struct {
    char board[3][3];
    char my_side;
    bool is_my_turn;
    bool win;
} core;

typedef struct {
    Command type;
    int sender;
    int recipient;
    int lastowner;
    char data[BUF_SIZE];
    int moreData;
    int messageID;
} message;


typedef struct {
    bool is_my_turn;
    char my_figure;
    char client_type[20];
} player_info;

void player_info_initialise(player_info* pl, char figure, const char* type);

void zmq_message_init(zmq_msg_t* mes, int sender, int recipient, int lastowner, Command command, char* data, int moreData, int messageID);

void message_init(message* mes, int sender, int recipient, int lastowner, Command command, char* data, int moreData, int messageID);

void message_standart(zmq_msg_t* mes, int sender, int recipient, Command command, char* data);