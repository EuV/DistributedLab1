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

int getNumberOfProcess( int argc, char * const argv[] );
void ChildProcess();
void ParentProcess();
int pipes[ MAX_PROCESS_ID ][ MAX_PROCESS_ID ][ 2 ];
int events;
int nProc;
local_id localId;

int main( int argc, char * argv[] ) {

	events = open( evengs_log, O_CREAT | O_TRUNC | O_WRONLY | O_APPEND );
	nProc = getNumberOfProcess( argc, argv );

	// Create fully-connected topology with excess of duplex pipes
	for ( int row = 0; row <= nProc; row++ ) {
		for ( int col = 0; col <= nProc; col++ ) {
			if ( row == col ) continue;
			pipe( pipes[ row ][ col ] );
		}
	}

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

	//    | 0 | i | 2 | 3
	// -------------------
	//  0 | - | r | - | -
	// -------------------
	//  i | w | - | w | w
	// -------------------
	//  2 | - | r | - | -
	// -------------------
	//  3 | - | r | - | -

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
	char buf[ BUF_SIZE ];

	// "Process %1d (pid %5d, parent %5d) has STARTED\n";
	sprintf( buf, log_started_fmt, localId, PID, pPID );
	write( 1, buf, strlen( buf ) );
	write( events, buf, strlen( buf ) );


	Message msg;
	send_multicast( NULL, &msg );


	// "Process %1d has DONE its work\n";
	sprintf( buf, log_done_fmt, localId );
	write( 1, buf, strlen( buf ) );
	write( events, buf, strlen( buf ) );

	// Close other pipes
	for ( int i = 0; i <= nProc; i++ ) {
		if ( i == localId ) continue;
		close( pipes[ localId ][ i ][ 1 ] );
		close( pipes[ i ][ localId ][ 0 ] );
	}
}


void ParentProcess() {

	//    | p | 1 | 2 | 3
	// -------------------
	//  p | - | - | - | -
	// -------------------
	//  1 | r | - | - | -
	// -------------------
	//  2 | r | - | - | -
	// -------------------
	//  3 | r | - | - | -

	// Close unused pipes
	for ( int row = 0; row <= nProc; row++ ) {
		for ( int col = 1; col <= nProc; col++ ) {
			if ( row == col ) continue;
			close( pipes[ row ][ col ][ 0 ] );
			close( pipes[ row ][ col ][ 1 ] );
		}
		close( pipes[ row ][ localId ][ 1 ] );
	}

	// Receive test message from all the children
	for ( int i = 1; i <= nProc; i++ ) {
		char tb[3];
		read( pipes[ i ][ 0 ][ 0 ], &tb, 3 );
		printf( "-%1d-Receive message: %3s\n", i, tb );
	}

	// Waiting for all the children
	while( wait( NULL ) != -1 ) { }

	// Close other pipes
	for ( int row = 1; row <= nProc; row++ ) {
		close( pipes[ row ][ 0 ][ 0 ] );
	}

	printf("*** Parent is done ***\n");
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


int send_multicast( void * self, const Message * msg ) {
	for ( int recipient = PARENT_ID; recipient <= nProc; recipient++ ) {
		if ( recipient == localId ) continue;
		write( pipes[ localId ][ recipient ][ 1 ], "OK", 3 );
	}
	return 0;
}
