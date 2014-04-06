#include "pa1.h"
#include "ipc.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <getopt.h>

#define NUMBER_OF_PROCESS 2
#define BUF_SIZE 100
#define IPC_SUCCESS 0
#define IPC_FAILURE -1

int getNumberOfProcess( int argc, char * const argv[] );
void ChildProcess();
void ParentProcess();
void makePipeLog();
int pipes[ MAX_PROCESS_ID + 1 ][ MAX_PROCESS_ID + 1][ 2 ];
int eventsLog;
int pipesLog;
int nProc;
local_id localId;


int main( int argc, char * argv[] ) {

	eventsLog = open( evengs_log, O_CREAT | O_TRUNC | O_WRONLY | O_APPEND );
	pipesLog = open( pipes_log, O_CREAT | O_TRUNC | O_WRONLY | O_APPEND );

	nProc = getNumberOfProcess( argc, argv );


	// Create fully-connected topology with excess of duplex pipes
	for ( int row = 0; row <= nProc; row++ ) {
		for ( int col = 0; col <= nProc; col++ ) {
			if ( row == col ) continue;
			pipe( pipes[ row ][ col ] );
		}
	}

	makePipeLog();

	// Create a brood of process
	for ( int i = 1; i <= nProc; i++ ) {
		if ( fork() == 0 ) {
			localId = i;
			ChildProcess();
			break; // Not to do fork() from the child
		} else if ( i == nProc ) { // The last child has been created
			localId = PARENT_ID;
			ParentProcess();
		}
	}

	return 0;
}


void ChildProcess() {

	// Close unused pipes
	for ( int row = 0; row <= nProc; row++ ) {
		for ( int col = 0; col <= nProc; col++ ) {
			if ( row == col ) continue;
			if ( row == localId ) {
				close( pipes[ row ][ col ][ 0 ] );
			} else {
				close( pipes[ row ][ col ][ 1 ] );
				if ( col != localId ) {
					close( pipes[ row ][ col ][ 0 ] );
				}
			}
		}
	}

	pid_t PID = getpid();
	pid_t pPID = getppid();

	// Create STARTED message
	Message sendMsg;
	sendMsg.s_header.s_magic = MESSAGE_MAGIC;
	sendMsg.s_header.s_payload_len = strlen( sendMsg.s_payload );
	sendMsg.s_header.s_type = STARTED;
	sprintf( sendMsg.s_payload, log_started_fmt, localId, PID, pPID );

	// Log STARTED message
	write( 1, sendMsg.s_payload, strlen( sendMsg.s_payload ) );
	write( eventsLog, sendMsg.s_payload, strlen( sendMsg.s_payload ) );

	// Send STARTED message to all the neighbours
	if ( send_multicast( NULL, &sendMsg ) == IPC_FAILURE ) {
		printf( "Multicast error in Process %d (%d)\n", localId, PID );
		exit( 1 );
	}


	// Receive STARTED message from all the neighbours
	Message receivedMsg;
	int startedProcCounter = 1; // For this one has already been started
	while ( startedProcCounter != nProc ) {
		receive_any( NULL, &receivedMsg );
		// TODO: if ( receivedMsg.s_header.s_type == STARTED ) { ...
		// TODO: if ( result != 0 ) { ...
		startedProcCounter++;
	}


	// Close other pipes
	for ( int i = 0; i <= nProc; i++ ) {
		if ( i == localId ) continue;
		close( pipes[ localId ][ i ][ 1 ] );
		close( pipes[ i ][ localId ][ 0 ] );
	}

	// printf( "The last sigh of %d (%d) process\n", localId, getpid() );
}


void ParentProcess() {

	// Close unused pipes
	for ( int row = 0; row <= nProc; row++ ) {
		for ( int col = 1; col <= nProc; col++ ) {
			if ( row == col ) continue;
			close( pipes[ row ][ col ][ 0 ] );
			close( pipes[ row ][ col ][ 1 ] );
		}
		close( pipes[ row ][ localId ][ 1 ] );
	}


	// Receive STARTED message from all the children
	Message receivedMsg;
	int startedProcCounter = 0;
	while ( startedProcCounter != nProc ) {
		receive_any( NULL, &receivedMsg );
		// TODO: if ( receivedMsg.s_header.s_type == STARTED ) { ...
		// TODO: if ( result != 0 ) { ...
		startedProcCounter++;
	}

	// Waiting for all the children
	int status;
	pid_t pid;
	while ( ( pid = wait( &status ) ) != -1 ) {
		printf( "Process %d has been done with exit code %d\n", pid, status );
		if( WIFSIGNALED( status ) ) printf( "!!! Interrupted by signal %d !!!\n", WTERMSIG( status ) );
    }

	// Close other pipes
	for ( int row = 1; row <= nProc; row++ ) {
		close( pipes[ row ][ 0 ][ 0 ] );
	}

	printf("*** Parent has been done ***\n");
}


int getNumberOfProcess( int argc, char * const argv[] ) {

	opterr = 0;

	int numberOfProcess = 0;
	int opt;

	while ( ( opt = getopt( argc, argv, "p:" ) ) != -1 ) {
		switch ( opt ) {
		case 'p':
			numberOfProcess = atoi( optarg );
			break;
		};
	};

	if ( numberOfProcess == 0 || numberOfProcess > MAX_PROCESS_ID ) {
		printf( "Set the default value for the number of child process: %d\n", NUMBER_OF_PROCESS );
		numberOfProcess = NUMBER_OF_PROCESS;
	}

	return numberOfProcess;
}



void makePipeLog() {

	char buf[ 10 ];

	for ( int row = 0; row <= nProc; row++ ) {

		for ( int col = 0; col <= nProc; col++ ) {
			sprintf( buf, "| %3d %3d ", pipes[ row ][ col ][ 0 ], pipes[ row ][ col ][ 1 ] );
			write( pipesLog, buf, strlen( buf ) );
		}

		write( pipesLog, "|\n", 2 );
	}
}


int send( void * self, local_id dst, const Message * msg) {

	ssize_t wasWrite = write( pipes[ localId ][ dst ][ 1 ], msg, sizeof( Message ) );

	// printf( "Send %d bytes from %d to %d\n", wasWrite, localId, dst );

	return ( wasWrite > 0 ) ? IPC_SUCCESS : IPC_FAILURE;
}



int send_multicast( void * self, const Message * msg ) {

	int status = IPC_SUCCESS;

	for ( local_id dst = PARENT_ID; dst <= nProc; dst++ ) {
		if ( dst == localId ) continue;
		status = send( NULL, dst, msg );
		if ( status == IPC_FAILURE ) break;
	}

	return status;
}



int receive( void * self, local_id from, Message * msg ) {

	ssize_t wasRead = read( pipes[ from ][ localId ][ 0 ], msg, sizeof( Message ) );

	// if( wasRead > 0 ) printf( "Receive %d bytes by %d proc from %d proc\n", wasRead, localId, from );

	return ( wasRead > 0 ) ? IPC_SUCCESS : IPC_FAILURE;
}



int receive_any( void * self, Message * msg ) {

	static local_id sender = PARENT_ID + 1;

	if( sender == localId ) sender++;

	// printf( "receive_any by %d proc from %d proc\n", localId, sender );

	return receive( NULL, sender++, msg );
}
