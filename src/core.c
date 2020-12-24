
#include "game.h"


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
        send_move(x, y, p, c);
        send_win(p);
        return c->win;
    }
    if (who == c->my_side) {
        send_move(x, y, p, c);
        c->is_my_turn = false;
    }
    else {
        c->is_my_turn = true;
    }
    return NO_WIN;
}