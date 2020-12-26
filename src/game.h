#pragma once

#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assert.h>
#include <ncurses.h>
#include <pthread.h>
#include <zmq.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "structs.h"

#define SQ_HEIGHT 7
#define SQ_WIDTH 9
#define BOARD_SIZE 9




#define CHAT_HEIGHT SQ_HEIGHT * 3 - 3

#define NO_WIN 2
#define WIN 1
#define CANT_SET 0




typedef struct {
    WINDOW* BOARD[10];

    void* INTEFACE_CONTEXT;
    void* TO_ROUTER;
    void* TASKS;
    bool chat_is_enabled;
    char chat_buf[CHAT_HEIGHT][BUF_SIZE];
} parts;


void client();

void server();

void interface(void* information);

void router_module(void* information);

// game core 
void initialise_core(core* c, player_info* info);
int core_turn(core* c, int pos, parts* p, char who);


// inteface utils
int kbhit(void);
void system_message(parts* p, const char* mes);
void send_chat_message(parts* p, const char* buf);
void send_win(parts* p);
void send_move(int x, int y, parts* p, core* c);
void chat_push(parts* p, const char* mes);
void chat_connected(parts* p, const char* m);
void read_stats( my_stat* to_read );
void read_stats_str(char* buf);

void draw(WINDOW* w, char  what);
void draw_square(int sq, char what, parts* where);
void highlight_square(int sq, parts* where);

void create_board(parts* part);