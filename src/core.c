
#include "game.h"


void deinitialise_core( core* c ){
    for( int i = 0; i < c->size; i++ ){
        free( c->board[i] );  
    }
    free( c->board );
}

void initialise_core(core* c, player_info* info) {
    c->board = malloc( (info->size) * sizeof(char*) );
    assert( c->board != NULL );
    for( int i = 0; i < info->size; i++ ){
        c->board[i] = malloc( (info->size) * sizeof(char) );
        assert( c->board[i] != NULL );
    }
    c->win = false;
    c->is_my_turn = info->is_my_turn;
    c->my_side = info->my_figure;
    c->size = info->size;
    for (int i = 0; i < info->size; i++) {
        for (int j = 0; j < info->size; j++) {
            c->board[i][j] = ' ';
        }
    }
}


bool check_case(core* c, int pos_x, int pos_y, int dx, int dy) {
    int count_right = 0;
    int curpos_x = pos_x, curpos_y = pos_y;
    while (curpos_x >= 0 && curpos_y >= 0 && curpos_x < c->size && curpos_y < c->size ) {
        if (c->board[curpos_x][curpos_y] == c->my_side) {
            count_right++;
        } else {
            return false;
        }
        curpos_x += dx;
        curpos_y += dy;
    }
    curpos_x = pos_x - dx; curpos_y = pos_y - dy;
    while (curpos_x < c->size && curpos_y < c->size && curpos_x >= 0 && curpos_y >= 0) {
        if (c->board[curpos_x][curpos_y] == c->my_side) {
            count_right++;
        } else {
            return false;
        }
        curpos_x -= dx;
        curpos_y -= dy;
    }
    return count_right == c->size;
}


bool  check_board(core* c, int x, int y) {
    if (check_case(c, x, y, -1, 0)){
        return true;
    }
    if (check_case(c, x, y, -1, -1)){
        return true;
    }
    if (check_case(c, x, y, 0, -1)){
        return true;
    }
    if (check_case(c, x, y, 1, -1)){
        return true;
    }
    return false; 
}


int core_turn(core* c, int pos, parts* p, char who) {
    int x = pos / c->size;
    int y = pos % c->size;
    if (c->board[x][y] != ' ' && who == c->my_side) {
        return CANT_SET;
    }
    c->board[x][y] = who;
    draw_square(p->BOARD[pos], who, p);
    c->win = check_board(c, x, y);
    if (c->win) {
        send_move(x, y, p, c);
        send_win(p);
        return c->win;
    }
    if (who == c->my_side) {
        send_move(x, y, p, c);
        c->is_my_turn = false;
    } else {
        c->is_my_turn = true;
    }
    return NO_WIN;
}
