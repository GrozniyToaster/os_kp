#include "structs.h"

void zmq_message_init(zmq_msg_t* mes, int sender, int recipient, int lastowner, Command command, const char* data, int moreData, int messageID) {
    zmq_msg_init_size(mes, sizeof(message));
    message tmp;
    tmp.sender = sender;
    tmp.recipient = recipient;
    tmp.lastowner = lastowner;
    tmp.type = command;
    strcpy(tmp.data, data);
    tmp.moreData = moreData;
    tmp.messageID = messageID;
    memcpy(zmq_msg_data(mes), &tmp, sizeof(message));
}

void message_init(message* mes, int sender, int recipient, int lastowner, Command command, const char* data, int moreData, int messageID) {

    mes->sender = sender;
    mes->recipient = recipient;
    mes->lastowner = lastowner;
    mes->type = command;
    mes->moreData = moreData;
    mes->messageID = messageID;
    strcpy(mes->data, data);
}

void message_standart(zmq_msg_t* mes, int sender, int recipient, Command command, const char* data) {
    zmq_message_init(mes, sender, recipient, sender, command, data, 0, 0);
}


void player_info_initialise(player_info* pl, char figure, const char* type, const char* address) {
    pl->is_my_turn = (figure == 'x');
    pl->my_figure = figure;
    pl->how_game_ended = NOT_ENDED;
    strcpy(pl->client_type, type);
    strcpy(pl->address, address);
    get_hostinfo(pl);
}


void ports_init(ports* p, player_info* pl_info) {
    int control;
    p->CONTEXT = zmq_ctx_new();
    assert(p->CONTEXT != NULL);
    char address[50];
    sprintf(address, "tcp://%s:%d", pl_info->address, PORT);

    if (strcmp(pl_info->client_type, "server") == 0) {
        p->opponent = zmq_socket(p->CONTEXT, ZMQ_PAIR);
        assert(p->opponent != NULL);
        control = zmq_bind(p->opponent, address);
    }
    else {
        p->opponent = zmq_socket(p->CONTEXT, ZMQ_PAIR);
        assert(p->opponent != NULL);
        control = zmq_connect(p->opponent, address);
    }
    assert(control == 0);
    const int Wait_time = 1000;
    zmq_setsockopt(p->opponent, ZMQ_RCVTIMEO, &Wait_time, sizeof(int));
    zmq_setsockopt(p->opponent, ZMQ_SNDTIMEO, &Wait_time, sizeof(int));

    char my_task_socket_name[BUF_SIZE];
    sprintf(my_task_socket_name, "ipc://@%s/main_loop_tasks", pl_info->client_type);
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

void get_hostinfo(player_info* pl_info) {
    char host_buf[BUF_SIZE];
    char* ip_buf;
    struct hostent* host_entry;

    assert((gethostname(host_buf, sizeof(host_buf)) != -1));
    host_entry = gethostbyname(host_buf);
    assert(host_entry != NULL);

    ip_buf = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
    sprintf(pl_info->info, "%s %s", host_buf, ip_buf);
}

void my_stat_initialise(my_stat* st) {
    st->lose_o = 0;
    st->lose_x = 0;
    st->win_o = 0;
    st->win_x = 0;
    st->not_ended = 0;
}

