
#include <stdio.h>
#include <string.h>
#include "game.h"

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        printf("Usage <XO> [client|server]\n");
        exit(0);
    }
    int size = atoi(argv[2]);
    if (strcmp(argv[1], "server") == 0) {
        if (argc == 4) {

            server(size, argv[3]);
        } else {
            server(size, "*");
        }
    } else if (strcmp(argv[1], "client") == 0) {
        if (argc == 4) {
            client(size, argv[3]);
        } else {
            client(size, "localhost");
        }

    } else {
        printf("Undefined role\n");

    }
    exit(0);
}