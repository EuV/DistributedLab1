#include "ipc.h"

void fill_message( Message * msg, MessageType type, void * payload, size_t psize ) {
}


int send_to( const Connector * self, local_id dst, const Message * msg ) {
	return -1;
}


int sendall_msg( const Connector * self, const Message * msg ) {
	return -1;
}


int receive_from( const Connector * self, local_id from, Message ** msg_out ) {
	return -1;
}


int creat_fully_connected_topology( Connector ** connectors, size_t n, int nonblocking ) {
	return -1;
}
