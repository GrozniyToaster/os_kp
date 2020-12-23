
#include "game.h"
#include <error.h>

typedef struct {
    void* CONTEXT;
    void* opponent;
    void* tasks;
    void* to_send;
} ports;

void ports_init(ports* p, player_info* pl_info) {
    int control;
    p->CONTEXT = zmq_ctx_new();
    assert(p->CONTEXT != NULL);

    if (strcmp(pl_info->client_type, "server") == 0) {
        p->opponent = zmq_socket(p->CONTEXT, ZMQ_PAIR);
        assert(p->opponent != NULL);
        control = zmq_bind(p->opponent, "tcp://*:4040");
    }
    else {
        p->opponent = zmq_socket(p->CONTEXT, ZMQ_PAIR);
        assert(p->opponent != NULL);
        control = zmq_connect(p->opponent, "tcp://localhost:4040");
    }
    assert(control == 0);

    char my_task_socket_name[BUF_SIZE];
    sprintf(my_task_socket_name, "ipc://@%s/main_loop_tasks", pl_info->client_type);
    //printf("%s\n", my_task_socket_name);
    p->tasks = zmq_socket(p->CONTEXT, ZMQ_PUB);
    assert(p->tasks != NULL);
    control = zmq_bind(p->tasks, my_task_socket_name);
    assert(control == 0);

    char my_to_send_socket_name[BUF_SIZE];
    sprintf(my_to_send_socket_name, "ipc://@%s/to_send", pl_info->client_type);
    p->to_send = zmq_socket(p->CONTEXT, ZMQ_SUB);
    assert(p->to_send != NULL);
    control = zmq_bind(p->to_send, my_to_send_socket_name);
    assert(control == 0);
    zmq_setsockopt(p->to_send, ZMQ_SUBSCRIBE, 0, 0);
}

void router_module(void* information) {

    ports my_ports;
    ports_init(&my_ports, information);

    //router_initialise();

    //printf("Client Listing Starting...\n");

    while (1) {
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

            //TODO copy and send
        }
        zmq_msg_init_size(&task, sizeof(message));
        sizeof_message = zmq_msg_recv(&task, my_ports.to_send, ZMQ_DONTWAIT);
        if (sizeof_message > 0) {
            //printf("Router recv task own\n");
            zmq_msg_send(&task, my_ports.opponent, 0);
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
    zmq_close(my_ports.opponent);
    zmq_close(my_ports.to_send);
    zmq_ctx_destroy(my_ports.CONTEXT);
}
