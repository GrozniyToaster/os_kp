#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <ncurses.h>
#include <assert.h>
#include <zmq.h>

#include "structs.h"

#define SQ_HEIGHT 7
#define SQ_WIDTH 9
#define BOARD_SIZE 9

#define NO_WIN 2
#define WIN 1
#define CANT_SET 0

typedef struct {
    WINDOW* BOARD[10];

    void* INTEFACE_CONTEXT;
    void* TO_ROUTER;
    void* TASKS;
} parts;


void draw(WINDOW* w, char  what) {
    if (what == 'x') {
        for (int xi = 1, xj = SQ_HEIGHT, y = 0; y < SQ_HEIGHT; y++, xi++, xj--) {
            mvwaddch(w, y, xi, '#' | A_BOLD);
            mvwaddch(w, y, xj, '#' | A_BOLD);
        }
    }
    else if (what == 'o' | what == '0') {
        for (int i = 0; i < 3; i++) {
            mvwaddch(w, 2 + i, 1, '#' | A_BOLD);
            mvwaddch(w, 2 + i, SQ_WIDTH - 2, '#' | A_BOLD);
        }
        for (int i = 0; i < 5; i++) {
            mvwaddch(w, 1, 2 + i, '#' | A_BOLD);
            mvwaddch(w, SQ_HEIGHT - 2, 2 + i, '#' | A_BOLD);
        }
    }
    else {
        return;
    }
    wrefresh(w);
}

void draw_square(int sq, char what, parts* where) {
    if (what != 0) {
        draw(where->BOARD[sq], what);
    }
    box(where->BOARD[sq], 0, 0);
    wrefresh(where->BOARD[sq]);
}


void create_board(parts* part) {
    int i;
    int start_y, start_x;

    start_y = 0;
    start_x = 0;
    for (i = 0; i < 3; i++) {
        part->BOARD[i] = newwin(SQ_HEIGHT, SQ_WIDTH, start_y, start_x);
        start_x += SQ_WIDTH;
    }
    start_y = SQ_HEIGHT;
    start_x = 0;
    for (i = 3; i < 6; i++) {
        part->BOARD[i] = newwin(SQ_HEIGHT, SQ_WIDTH, start_y, start_x);
        start_x += SQ_WIDTH;
    }
    start_x = 0;
    start_y = 2 * SQ_HEIGHT;
    for (i = 6; i < 9; i++) {
        part->BOARD[i] = newwin(SQ_HEIGHT, SQ_WIDTH, start_y, start_x);
        start_x += SQ_WIDTH;
    }
    part->BOARD[9] = newwin(SQ_HEIGHT * 3, 50, 0, 3 * SQ_WIDTH);
    for (i = 0; i < 10; i++) {
        draw_square(i, 0, part);
    }
}

void highlight_square(int sq, parts* where) {
    wattron(where->BOARD[sq], A_STANDOUT);
    draw_square(sq, 0, where);
    wattroff(where->BOARD[sq], A_STANDOUT);
}

void initialise_core(core* c, player_info* info) {
    c->win = false;
    c->is_my_turn = info->is_my_turn;
    c->my_side = info->my_figure;
    for (int i = 0; i < BOARD_SIZE / 3; i++) {
        for (int j = 0; j < BOARD_SIZE / 3; j++) {
            c->board[i][j] = ' ';
        }
    }
}

bool  check_board(core* c, int x, int y) {
    // check horizontal
    for (int i = 0; i < 3; i++) {
        if (c->board[i][0] == c->board[i][1] &&
            c->board[i][1] == c->board[i][2] &&
            c->board[i][2] == c->my_side
            ) {
            return true;
        }
    }
    // check columns
    for (int i = 0; i < 3; i++) {
        if (c->board[0][i] == c->board[1][i] &&
            c->board[1][i] == c->board[2][i] &&
            c->board[2][i] == c->my_side
            ) {
            return true;
        }
    }
    // diagonals
    if (c->board[0][0] == c->board[1][1] &&
        c->board[1][1] == c->board[2][2] &&
        c->board[2][2] == c->my_side
        ) {
        return true;
    }
    if (c->board[2][1] == c->board[1][1] &&
        c->board[1][1] == c->board[0][2] &&
        c->board[0][2] == c->my_side
        ) {
        return true;
    }
    return false;
}

void send_win(parts* p) {
    zmq_msg_t to_send;
    message_standart(&to_send, 42, 1, OPPONENT_WIN, "opponent win");
    zmq_msg_send(&to_send, p->TO_ROUTER, 0);
    zmq_msg_close(&to_send);
}

void send_move(int x, int y, parts* p, core* c) {
    zmq_msg_t to_send;
    char data[256];
    sprintf(data, "%c %d %d", c->my_side, x, y);
    message_standart(&to_send, 402, 1, TURN, data);
    zmq_msg_send(&to_send, p->TO_ROUTER, 0);
    zmq_msg_close(&to_send);

}

int core_turn(core* c, int pos, parts* p, char who) {
    int x = pos / 3;
    int y = pos % 3;
    if (c->board[x][y] != ' ' && who == c->my_side) {
        return CANT_SET;
    }
    c->board[x][y] = who;
    draw_square(pos, who, p);
    c->win = check_board(c, x, y);
    if (c->win) {
        send_win(p);
        return c->win;
    }
    if (who == c->my_side) {
        send_move(x, y, p, c);
    }
    return NO_WIN;
}

void interface_initialise(parts* to_init, core* c, player_info* info) {
    to_init->INTEFACE_CONTEXT = zmq_ctx_new();
    assert(to_init->INTEFACE_CONTEXT != NULL);

    to_init->TO_ROUTER = zmq_socket(to_init->INTEFACE_CONTEXT, ZMQ_PUB);
    assert(to_init->TO_ROUTER != NULL);

    char router_name[BUF_SIZE];
    sprintf(router_name, "ipc://@%s/to_send", info->client_type);
    char tasks_sock_name[BUF_SIZE];
    sprintf(tasks_sock_name, "ipc://@%s/main_loop_tasks", info->client_type);


    int control = zmq_connect(to_init->TO_ROUTER, router_name);
    assert(control == 0);

    to_init->TASKS = zmq_socket(to_init->INTEFACE_CONTEXT, ZMQ_SUB);
    assert(to_init->TASKS != NULL);
    control = zmq_connect(to_init->TASKS, tasks_sock_name);
    assert(control == 0);
    zmq_setsockopt(to_init->TASKS, ZMQ_SUBSCRIBE, 0, 0);

    initialise_core(c, info);

    curs_set(0);
    initscr();
    noecho();
    cbreak();
    if ((LINES < 24) || (COLS < 80)) {
        endwin();
        puts("Your terminal needs to be at least 80x24");
        exit(2);
    }
    clear();
    refresh();
    create_board(to_init);
}

void parse(message* m, core* c, parts* p) {
    if (m->type == TURN) {
        char ch;
        int x, y;
        sscanf(m->data, "%c%d%d", &ch, &x, &y);
        //printf("Inteface  %d \n",x * 3 + y);

        core_turn(c, x * 3 + y, p, ch);

    }
    else if (m->type == OPPONENT_WIN) {
        //TODO OPPONENT WIN
    }
    //TODO CHAT

}

void check_task(core* c, parts* p) {
    zmq_msg_t to_recv;
    zmq_msg_init_size(&to_recv, sizeof(message));
    int size = zmq_msg_recv(&to_recv, p->TASKS, ZMQ_DONTWAIT);
    if (size > 0) {
        //printf("Toparse\n");
        message recv;
        memcpy(&recv, zmq_msg_data(&to_recv), sizeof(message));
        parse(&recv, c, p);
        zmq_msg_close(&to_recv);
    }
}


void interface(void* information) {
    player_info* info = information;
    parts this_interface;
    core my_core;
    interface_initialise(&this_interface, &my_core, info);
    int key;
    int sq;
    sq = 0;
    highlight_square(sq, &this_interface);
    do {
        check_task(&my_core, &this_interface);
        key = getch();
        if (key == '+' && sq < 9) {
            draw_square(sq, 0, &this_interface);
            highlight_square(++sq, &this_interface);
        }
        else if (key == ' ' && sq < 9) {
            if (!my_core.is_my_turn) {
                //TODO print 
                continue;
            }
            if (core_turn(&my_core, sq, &this_interface, my_core.my_side)) {
                //TODO check which turn
                draw_square(sq, my_core.my_side, &this_interface);
                highlight_square(sq, &this_interface);
                if (my_core.win) {
                    //TODO congratulations
                }
            }
        }
        else if (key == '-' && sq > 0) {
            draw_square(sq, 0, &this_interface);
            highlight_square(--sq, &this_interface);
        }
        else {
        }
    } while ((key != 'q') && (key != 'Q'));
}
