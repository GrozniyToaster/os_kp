#include <zmq.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>


#include "structs.h"

typedef struct MD
{
    int clientId;
    int messageNumber;
    char message[128];
} MessageData;

typedef struct {
    char sock_name[256];
    char* receive;
} thread_info;

void client_send_module(void* information) {
    void* context = zmq_ctx_new();
    printf("Client Listing Starting...\n");
    void* to_server = zmq_socket(context, ZMQ_REQ);
    zmq_connect(to_server, "tcp://localhost:4040");

    void* to_send = zmq_socket(context, ZMQ_SUB);
    int rc = zmq_bind(to_send, "ipc://@client/to_send");
    assert(rc == 0);
    zmq_setsockopt(to_send, ZMQ_SUBSCRIBE, 0, 0);
    while (1) {
        zmq_msg_t task;
        zmq_msg_init_size(&task, sizeof(message));
        int sizeof_message = zmq_msg_recv(&task, to_server, ZMQ_DONTWAIT);
        if (sizeof_message > 0) {
            message kek;
            memcpy(&kek, zmq_msg_data(&task), sizeof(message));
            printf("Сервер прислал %s\n", kek.data);
            zmq_msg_close(&task);
            //TODO parse
            //TODO copy and send
        }
        zmq_msg_init_size(&task, sizeof(message));
        sizeof_message = zmq_msg_recv(&task, to_send, ZMQ_DONTWAIT);
        if (sizeof_message > 0) {
            zmq_msg_send(&task, to_server, 0);
            zmq_msg_close(&task);
        }
        sleep(1);
    }
    zmq_close(to_server);
    zmq_close(to_send);
    zmq_ctx_destroy(context);
}

int interface() {
    void* context = zmq_ctx_new();
    void* to_send = zmq_socket(context, ZMQ_PUB);
    zmq_connect(to_send, "ipc://@client/to_send");
    while (1) {
        zmq_msg_t kek;
        char text[] = "Interface";
        message_standart(&kek, 42, 42, CHAT, text);
        zmq_msg_send(&kek, to_send, 0);
        sleep(2);
    }

}

int chat() {
    void* context = zmq_ctx_new();
    void* to_send = zmq_socket(context, ZMQ_PUB);
    int rc = zmq_connect(to_send, "ipc://@client/to_send");
    assert(rc == 0);
    while (1) {
        zmq_msg_t kek;
        char text[] = "chat";
        //printf("Client chat\n");
        message_standart(&kek, 42, 42, CHAT, text);
        zmq_msg_send(&kek, to_send, 0);
        sleep(3);
    }

}


int main() {
    pthread_t server_connect, interface_thread, chat_thread;
    thread_info serv_info, client_info;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&server_connect, &attr, (void*)client_send_module, &client_info);
    pthread_create(&interface_thread, &attr, (void*)interface, &client_info);
    pthread_create(&chat_thread, &attr, (void*)chat, &client_info);
    pthread_join(server_connect, NULL);
}