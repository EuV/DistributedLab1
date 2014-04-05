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

#define NUMBER_OF_PROCESS 6
#define BUF_SIZE 100

int getNumberOfProcess( int argc, char * const argv[] );
int getopt( int argc, char * const argv[], const char * optstring );
void ChildProcess( local_id localId );
void ParentProcess( local_id localId );
extern char *optarg;
extern int opterr;
int events;

int main( int argc, char * argv[] ) {

	events = open( evengs_log, O_CREAT | O_TRUNC | O_WRONLY | O_APPEND );
	int numberOfProcess = getNumberOfProcess( argc, argv );

	for ( int i = 1; i <= numberOfProcess; i++ ) {
		if ( fork() == 0 ) {
			ChildProcess( i );
			break; // Not to do fork() from the child
		} else if ( i == numberOfProcess ) { // The last child has been created
			ParentProcess( 0 );
		}
	}

	return 0;
}


void  ChildProcess( local_id localId ) {

	pid_t PID = getpid();
	pid_t pPID = getppid();
	char buf[ BUF_SIZE ];

	// "Process %1d (pid %5d, parent %5d) has STARTED\n";
	sprintf( buf, log_started_fmt, localId, PID, pPID );
	write( 1, buf, strlen( buf ) );
	write( events, buf, strlen( buf ) );

	// "Process %1d has DONE its work\n";
	sprintf( buf, log_done_fmt, localId );
	write( 1, buf, strlen( buf ) );
	write( events, buf, strlen( buf ) );
}


void  ParentProcess( local_id localId ) {

	// Waiting for all the children
	int status;
	while( wait( &status ) != -1 ) { }

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

	if ( numberOfProcess == 0 || numberOfProcess > 10 ) {
		printf( "Set the default value for the number of child process: %d\n", NUMBER_OF_PROCESS );
		numberOfProcess = NUMBER_OF_PROCESS;
	}

	return numberOfProcess;
}
