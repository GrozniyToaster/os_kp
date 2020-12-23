#include <stdio.h>
#include <zmq.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "structs.h"

#include "game.h"

/*
void serv_send_module(void* information) {
	IDCard* info;
	void* context = zmq_ctx_new();
	void* serverSocket = zmq_socket(context, ZMQ_REP);
	zmq_bind(serverSocket, "tcp://*:4040");
	printf("Starting...\n");
	for (;;) {
		zmq_msg_t mes;
		zmq_msg_init(&mes);
		zmq_msg_recv(&mes, serverSocket, 0);
		message* m = (message*)zmq_msg_data(&mes);
		printf("Message from client: %d message: %s\n", m->sender, m->data);
		zmq_msg_close(&mes);
		zmq_msg_t reply;
		message_standart(&reply, 777, 42, ANSWER, "server turn ended");
		//zmq_msg_init_size(&reply, strlen("server turn endede\0")+1);
		//memcpy(zmq_msg_data(&reply), "server turn ended\0", strlen("server turn endede\0")+1);
		zmq_msg_send(&reply, serverSocket, 0);
		zmq_msg_close(&reply);
	}
	zmq_close(serverSocket);
	zmq_ctx_destroy(context);
}
*/


int main() {
	pthread_t server_connect, interface_thread, chat_thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	player_info serv_info, client_info;
	player_info_initialise(&client_info, 'o', "server");
	pthread_create(&server_connect, &attr, (void*)router_module, &client_info);

	interface(&client_info);

	pthread_join(server_connect, NULL);

















	/*

	pthread_t thread_server, thread_client;
	IDCard serv_info, client_info;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&thread_server, &attr, (void*)serv_send_module, &serv_info);
	//pthread_create(&thread_client, &attr, client_module, &client_info );

	pthread_join(thread_server, NULL);
	//pthread_join(thread_client, NULL);


*/
}