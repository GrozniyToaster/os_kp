
#include <stdio.h>
#include <string.h>
#include "game.h"

int main( int argc, char* argv[] ){
    if ( argc < 2 || argc > 3 ){
        printf("Usage <XO> [client|server]\n");
        exit(0);
    }
    if ( strcmp( argv[1], "server" ) == 0 ){
        server();
    }else if ( strcmp( argv[1], "client" ) == 0 ){
        client();
    }else{
        printf( "Undefined role\n" );

    }
    exit(0);
}