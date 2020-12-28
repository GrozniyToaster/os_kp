#include "game.h"



void server(int size, const char* address) {
	pthread_t server_connect;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	player_info client_info;
	player_info_initialise(&client_info, 'x', "server", address, size);
	pthread_create(&server_connect, &attr, (void*)router_module, &client_info);

	interface(&client_info);

	pthread_join(server_connect, NULL);

}
