cmake_minimum_required(VERSION 3.0.0)
project(OS_KP VERSION 0.1.0)

include(CTest)
enable_testing()

add_definitions( -g3 -Wall )

add_executable(XO main.c server.c client.c structs.c inteface.c interface_utils.c core.c router.c  )


target_link_libraries( XO PRIVATE
    pthread
    zmq
    ncurses
)


