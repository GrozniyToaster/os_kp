#include "game.h"


int main() {
	pthread_t server_connect, interface_thread, chat_thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	player_info serv_info, client_info;
	player_info_initialise(&client_info, 'x', "server");
	pthread_create(&server_connect, &attr, (void*)router_module, &client_info);

	interface(&client_info);

	pthread_join(server_connect, NULL);

}