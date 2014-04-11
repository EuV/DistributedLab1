#include "la1.h"

int send( void * self, local_id dst, const Message * msg) {

	Process * proc = self;

	ssize_t wasWrite = write( Pipes[ proc -> localId ][ dst ][ WRITE ], msg, sizeof( MessageHeader ) +
		msg -> s_header.s_payload_len );

	// printf( "Send %d bytes from %d to %d\n", wasWrite, proc -> localId, dst );

	return ( wasWrite > 0 ) ? IPC_SUCCESS : IPC_FAILURE;
}


int send_multicast( void * self, const Message * msg ) {

	Process * proc = self;

	int status = IPC_SUCCESS;

	for ( local_id dst = PARENT_ID; dst <= proc -> total; dst++ ) {
		if ( dst == proc -> localId ) continue;
		status = send( self, dst, msg );
		if ( status == IPC_FAILURE ) break;
	}

	return status;
}


int receive( void * self, local_id from, Message * msg ) {

	Process * proc = self;

	// Read the header of the message (default size)
	ssize_t wasRead = read( Pipes[ from ][ proc -> localId ][ READ ], &( msg -> s_header ), sizeof( MessageHeader ) );

	// Read the rest part of the message which size has been known from the header
	wasRead += read( Pipes[ from ][ proc -> localId ][ READ ], &( msg -> s_payload ), msg -> s_header.s_payload_len );

	// if( wasRead > 0 ) printf( "Receive %d bytes by %d proc from %d proc\n", wasRead, proc -> localId, from );

	return ( wasRead > 0 ) ? IPC_SUCCESS : IPC_FAILURE;
}


int receive_any( void * self, Message * msg ) {

	Process * proc = self;

	static local_id sender = PARENT_ID + 1;

	if( sender == proc -> localId ) {
		if( sender < proc -> total ) {
			sender++;
		} else {
			sender = PARENT_ID + 1;
		}
	}

	// printf( "receive_any by %d proc from %d proc\n", proc -> localId, sender );
	int status = receive( self, sender++, msg );
	if( sender > proc -> total ) sender = PARENT_ID + 1;
	return status;
}
