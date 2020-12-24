#pragma once

#include <stdbool.h>
#include <assert.h>
#include <string.h> 
#include <zmq.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define BUF_SIZE 256
#define MAIN_MODULE -1
#define TO_ALL -42
#define FIRST_WATCHED -87

#define MORE_DATA 1

#define PORT 4040

typedef enum {
    REPLY,
    CONNECT,
    CHAT,
    ANSWER,
    PING,
    DATA,
    TURN,
    ERROR,
    OPPONENT_WIN,
    QUIT
} Command;




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
    char board[3][3];
    char my_side;
    bool is_my_turn;
    bool win;
} core;

typedef struct {
    bool is_my_turn;
    char my_figure;
    char client_type[20];
    char address[20];
    char info[256];
} player_info;

typedef struct {
    void* CONTEXT;
    void* opponent;
    void* tasks;
    void* to_send;
} ports;


void zmq_message_init(zmq_msg_t* mes, int sender, int recipient, int lastowner, Command command,const char* data, int moreData, int messageID);
void message_init(message* mes, int sender, int recipient, int lastowner, Command command,const char* data, int moreData, int messageID);
void message_standart(zmq_msg_t* mes, int sender, int recipient, Command command,const char* data);

void player_info_initialise(player_info* pl, char figure, const char* type, const char* address);

void ports_init(ports* p, player_info* pl_info);
void get_hostinfo( player_info* pl_info );
