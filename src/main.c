
#include <stdio.h>
#include <string.h>
#include "game.h"

int main( int argc, char* argv[] ){
    if ( argc < 2 || argc > 3 ){
        printf("Usage <XO> [client|server]\n");
        exit(0);
    }
    if ( strcmp( argv[1], "server" ) == 0 ){
        if ( argc == 3 ){
            server( argv[2] );
        }else{
            server("*");
        }
    }else if ( strcmp( argv[1], "client" ) == 0 ){
        if ( argc == 3 ){
            client( argv[2] );
        }else{
            client("localhost");
        }

    }else{
        printf( "Undefined role\n" );

    }
    exit(0);
}