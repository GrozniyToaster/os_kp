#include "game.h"

/**
 * @brief check is keyboard hit (key was pressed )
 *
 * @return int (true or false)
 */
int kbhit(void) {
    int ch, r;
    nodelay(stdscr, true);
    ch = getch();
    if (ch == ERR) {
        r = false;
    } else {
        r = true;
        ungetch(ch);
    }
    nodelay(stdscr, false);
    return r;
}

void system_message(parts* p, const char* mes) {
    char formatted[256];
    sprintf(formatted, "GameMaster: %s", mes);
    chat_push(p, formatted);
}


void send_chat_message(parts* p, const char* buf) {
    zmq_msg_t to_send;
    message_standart(&to_send, FIRST_WATCHED, ANONIMUS, CHAT, buf);
    zmq_msg_send(&to_send, p->TO_ROUTER, 0);
}

void chat_push(parts* p, const char* mes) {
    wclear(p->CHAT);
    highlight_square(p->CHAT, p);
    for (int i = CHAT_HEIGHT - 1; i > 0; i--) {
        strcpy(p->chat_buf[i], p->chat_buf[i - 1]);
    }
    strcpy(p->chat_buf[0], mes);
    for (int i = CHAT_HEIGHT, chat_str = 0; i > 0; i--, chat_str++) {
        mvwprintw(p->CHAT, i, 1, "%s", p->chat_buf[chat_str], chat_str, i);
    }
    wrefresh(p->CHAT);
}

void read_stats(my_stat* to_read) {
    FILE* read_stat = fopen("/tmp/xo_stat/stat.bin", "rb");
    if (read_stat == NULL) { return; }
    fread(to_read, sizeof(my_stat), 1, read_stat);
    fclose(read_stat);

}

void read_stats_str(char* buf) {
    my_stat st;
    read_stats(&st);
    sprintf(buf, "Win X %d; Win O %d; Loses X %d; Loses X %d; Not ended %d", st.win_x, st.win_o, st.lose_x, st.lose_o, st.not_ended );
}

void chat_connected(parts* p, const char* m) {
    char buf[BUF_SIZE];
    sprintf(buf, "Connected %s", m);
    chat_push(p, buf);
}

void send_win(parts* p) {
    zmq_msg_t to_send;
    message_standart(&to_send, FIRST_WATCHED, ANONIMUS, OPPONENT_WIN, "opponent win");
    zmq_msg_send(&to_send, p->TO_ROUTER, 0);
    zmq_msg_close(&to_send);
}

void send_move(int x, int y, parts* p, core* c) {
    zmq_msg_t to_send;
    char data[256];
    sprintf(data, "%c %d %d", c->my_side, x, y);
    message_standart(&to_send, FIRST_WATCHED, ANONIMUS, TURN, data);
    zmq_msg_send(&to_send, p->TO_ROUTER, 0);
    zmq_msg_close(&to_send);
}

void draw(WINDOW* w, char  what) {
    mvwaddch(w, 1, 1, what | A_BOLD);
    wrefresh(w);
}

void draw_square(WINDOW* w, char what, parts* where) {
    if (what != 0) {
        draw(w, what);
    }
    box(w, 0, 0);
    wrefresh(w);
}


void create_board(parts* part, player_info* info) {
    int size_of_boar = (info->size) * (info->size);
    part->BOARD = malloc( size_of_boar * sizeof( WINDOW* ) );
    
    int start_y, start_x;

    start_y = 0;
    for ( int j = 0; j < info->size; j++ ){
        start_x = 0;
        for ( int i = 0; i < info->size; i++) {
            part->BOARD[j * (info->size) + i] = newwin(SQ_HEIGHT, SQ_WIDTH, start_y, start_x);
            start_x += SQ_WIDTH;
        }
        start_y += SQ_HEIGHT;
    }
    part->CHAT = newwin(30, 70, 0, (info->size) * SQ_WIDTH);
    for (int i = 0; i < size_of_boar; i++) {
        draw_square(part->BOARD[i], 0, part);
    }
    draw_square(part->CHAT, 0, part);

}


void highlight_square(WINDOW* w, parts* where) {
    wattron(w, A_STANDOUT);
    draw_square(w, 0, where);
    wattroff(w, A_STANDOUT);

}
