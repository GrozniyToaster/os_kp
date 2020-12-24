#include "game.h"

void router_deinit( ports* to_deinit ){
    zmq_close(to_deinit->opponent);
    zmq_close(to_deinit->to_send);
    zmq_close( to_deinit->tasks );
    zmq_ctx_destroy(to_deinit->CONTEXT);
}


void router_module(void* information) {
    ports my_ports;
    ports_init(&my_ports, information);

    while (true) {
        zmq_msg_t task;
        zmq_msg_init_size(&task, sizeof(message));
        int sizeof_message = zmq_msg_recv(&task, my_ports.opponent, ZMQ_DONTWAIT);
        if (sizeof_message > 0) {
            message kek;
            zmq_msg_t try;
            zmq_msg_init_size(&try, sizeof(message));
            //printf("Router recv opponent\n");
            memcpy((&try), zmq_msg_data(&task), sizeof(message));
            //printf("Router %s\n", kek.data);
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
            //printf("Router recv task own\n");
            zmq_msg_send(&task, my_ports.opponent, 0);
            if ( ((message*)zmq_msg_data(&task)) -> type == QUIT ){
                printf("exit");
                break;
            }
            zmq_msg_close(&task);
            zmq_msg_t ans;
            sleep(1);
            zmq_msg_init(&ans);
            zmq_msg_recv(&ans, my_ports.opponent, 0);
            zmq_msg_close(&ans);
        }
        //printf("loop\n");
        sleep(1);

    }
    router_deinit( &my_ports );
}
