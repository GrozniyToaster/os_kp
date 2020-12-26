#include "game.h"

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

    to_init->chat_is_enabled=false;
    for ( int  i = 0 ; i < CHAT_HEIGHT; i++  ){
        to_init->chat_buf[i][0] = '\0';
    }
    mvprintw( LINES - 1, 0, info->info );
}

void deinitialize( parts* to_deinit ){
    zmq_msg_t to_exit;
    message_standart( &to_exit, FIRST_WATCHED, ANONIMUS, QUIT, "" );
    zmq_msg_send(&to_exit, to_deinit->TO_ROUTER, 0);
    
    zmq_close(to_deinit->TO_ROUTER);
    zmq_close(to_deinit->TASKS);
    zmq_ctx_destroy(to_deinit->INTEFACE_CONTEXT);
    endwin();
    
}

void parse(message* m, core* c, parts* p) {
    if (m->type == TURN) {
        char ch;
        int x, y;
        sscanf(m->data, "%c%d%d", &ch, &x, &y);
        core_turn(c, x * 3 + y, p, ch);
    } else if (m->type == OPPONENT_WIN) {
        system_message(p, "opponent win");
    } else if (m->type == CHAT) {
        char formatted[BUF_SIZE];
        sprintf( formatted, "opponent: %s", m->data );
        chat_push( p, formatted );
    } else if ( m->type == QUIT ){
        system_message( p, "opponent exited from game" );
    } else if ( m->type == CONNECT ){
        chat_connected( p, m->data );
    }

}

void check_task(core* c, parts* p) {
    zmq_msg_t to_recv;
    zmq_msg_init_size(&to_recv, sizeof(message));
    int size = zmq_msg_recv(&to_recv, p->TASKS, ZMQ_DONTWAIT);
    if (size > 0) {
        message recv;
        memcpy(&recv, zmq_msg_data(&to_recv), sizeof(message));
        parse(&recv, c, p);
        zmq_msg_close(&to_recv);
    }
}


void chat_enable( parts* p  ){
    p->chat_is_enabled = true;
    char buf[BUF_SIZE];
    char formatted[BUF_SIZE ];
    echo();
    curs_set(1);
    mvwgetnstr(p->BOARD[9], CHAT_HEIGHT+1, 1,buf, BUF_SIZE);
    sprintf( formatted,"me: %s", buf );
    chat_push( p , formatted );        
    noecho();
    curs_set(0);
    send_chat_message( p, buf );
    p->chat_is_enabled = false;
}

void first_message( parts* p, player_info* info ){
    char fm[BUF_SIZE];
    sprintf( fm , "you figure is %c", info->my_figure );
    system_message( p, fm );
    if ( info->is_my_turn ){
        sprintf( fm, "now your turn"  );
    }else {
        sprintf( fm, "now opponents turn" );
    }
    system_message(p, fm);
}

void interface(void* information) {
    player_info* info = information;
    parts this_interface;
    core my_core;
    interface_initialise(&this_interface, &my_core, info);
    int key = '\0';
    int sq = 0;
    
    highlight_square(sq, &this_interface);
    first_message( &this_interface, info );
    do {
        check_task(&my_core, &this_interface);
        if (!kbhit()) {
            continue;
        }
        key = getch();
        if (key == '+' && sq < 9) {
            draw_square(sq, 0, &this_interface);
            ++sq;
            highlight_square(sq, &this_interface);
            
        } else if (key == ' ' ) {
            if ( sq == 9 ){
                chat_enable( &this_interface );
            }
            if (!my_core.is_my_turn) {
                system_message( &this_interface, "not you turn" );
                continue;
            }
            if (core_turn(&my_core, sq, &this_interface, my_core.my_side)) {
                    draw_square(sq, my_core.my_side, &this_interface);
                    highlight_square(sq, &this_interface);
                    if (my_core.win) {
                        system_message( &this_interface, "Are u wining son?" );
                    }
            }else{
                system_message( &this_interface,"you cant set figure there");
            }
        }else if (key == '-' && sq > 0) {
            draw_square(sq, 0, &this_interface);
            highlight_square(--sq, &this_interface);
        }
    } while ((key != 'q') && (key != 'Q'));
    deinitialize( &this_interface  );
}
