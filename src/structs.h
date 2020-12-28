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
#define ANONIMUS -1

#define NOT_ENDED -18
#define I_WIN 10
#define I_LOSE 44

#define MORE_DATA 1

#define PORT 4040

typedef enum {
    FIRST_CONNECT,
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
    char** board;
    char my_side;
    bool is_my_turn;
    bool win;
    int size;
} core;

typedef struct {
    bool is_my_turn;
    char my_figure;
    int how_game_ended;
    char client_type[20];
    char address[20];
    char info[256];
    int size;
} player_info;

typedef struct {
    void* CONTEXT;
    void* opponent;
    void* tasks;
    void* to_send;
} ports;


typedef struct {
    int win_x;
    int lose_x;
    int win_o;
    int lose_o;
    int not_ended;
} my_stat;

void zmq_message_init(zmq_msg_t* mes, int sender, int recipient, int lastowner, Command command, const char* data, int moreData, int messageID);
void message_init(message* mes, int sender, int recipient, int lastowner, Command command, const char* data, int moreData, int messageID);
void message_standart(zmq_msg_t* mes, int sender, int recipient, Command command, const char* data);

void player_info_initialise(player_info* pl, char figure, const char* type, const char* address, int size_of_board);

void ports_init(ports* p, player_info* pl_info);
void get_hostinfo(player_info* pl_info);

void my_stat_initialise(my_stat* st);
