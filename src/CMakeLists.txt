cmake_minimum_required(VERSION 3.0.0)
project(OS_KP VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(client client.c structs.c inteface.c )
add_executable(server server.c structs.c)


target_link_libraries( server PRIVATE
    pthread
    zmq
)
target_link_libraries( client PUBLIC
    pthread
    zmq
    ncurses
)


include(CPack)
