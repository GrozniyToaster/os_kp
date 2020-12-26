#include "game.h"

void router_deinit( ports* to_deinit ){
    zmq_close(to_deinit->opponent);
    zmq_close(to_deinit->to_send);
    zmq_close( to_deinit->tasks );
    zmq_ctx_destroy(to_deinit->CONTEXT);
}


void send_first_information( ports* p, player_info* info ){
    if ( strcmp(info ->client_type, "server") == 0  ){
        char st[BUF_SIZE];
        read_stats_str(st);
        zmq_msg_t to_answer;
        message_standart(&to_answer, FIRST_WATCHED, ANONIMUS, CONNECT,st );
        zmq_msg_send( &to_answer, p->opponent, 0 );
        return;
    }
    zmq_msg_t to_send;
    message_standart( &to_send, FIRST_WATCHED, ANONIMUS, CONNECT, info->info );
    zmq_msg_send( &to_send, p->opponent, 0 );
}


void router_module(void* information) {
    ports my_ports;
    player_info* info = (player_info*)information; 
    ports_init(&my_ports, info);
    send_first_information( &my_ports, info );

    while (true) {
        zmq_msg_t task;
        zmq_msg_init_size(&task, sizeof(message));
        int sizeof_message = zmq_msg_recv(&task, my_ports.opponent, ZMQ_DONTWAIT);
        if (sizeof_message > 0) {
            zmq_msg_send(&task, my_ports.tasks, 0);
            zmq_msg_close(&task);
            sleep(1);

            zmq_msg_t ans;
            zmq_msg_init(&ans);
            zmq_msg_send(&ans, my_ports.opponent, 0);
            zmq_msg_close(&ans);
        }
        zmq_msg_init_size(&task, sizeof(message));
        sizeof_message = zmq_msg_recv(&task, my_ports.to_send, ZMQ_DONTWAIT);
        if (sizeof_message > 0) {
            if ( ((message*)zmq_msg_data(&task)) -> type == QUIT ){
                zmq_msg_send(&task, my_ports.opponent, 0);
                break;
            }else{
                zmq_msg_send(&task, my_ports.opponent, 0);
            }
            zmq_msg_close(&task);
            zmq_msg_t ans;
            sleep(1);
            zmq_msg_init(&ans);
            zmq_msg_recv(&ans, my_ports.opponent, 0);
            zmq_msg_close(&ans);
        }
        sleep(1);

    }
    router_deinit( &my_ports );
}
