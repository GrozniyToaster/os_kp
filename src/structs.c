#include "structs.h"

void zmq_message_init( zmq_msg_t* mes, int sender, int recipient, int lastowner, Command command, char* data, int moreData, int messageID  ){
    zmq_msg_init_size( mes, sizeof(message) );
    message tmp;
    tmp.sender = sender;
    tmp.recipient = recipient;
    tmp.lastowner = lastowner;
    tmp.type = command;
    strcpy( tmp.data, data );
    tmp.moreData = moreData;
    tmp.messageID = messageID;
    memcpy( zmq_msg_data(mes), &tmp, sizeof(message) );
}

void message_init( message* mes, int sender, int recipient, int lastowner, Command command, char* data, int moreData, int messageID  ){

    mes -> sender = sender;
    mes -> recipient = recipient;
    mes -> lastowner = lastowner;
    mes -> type = command;
    mes -> moreData = moreData;
    mes -> messageID = messageID;
    strcpy( mes -> data, data );
}

void message_standart( zmq_msg_t* mes, int sender ,int recipient, Command command, char* data ) {
    zmq_message_init( mes, sender, recipient, sender, command, data, 0, 0 );
} 