#include "pa1.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include  <sys/types.h>

#define NUMBER_OF_PROCESS 6

int getNumberOfProcess( int argc, char * const argv[] );
int getopt( int argc, char * const argv[], const char * optstring );
void ChildProcess();
void ParentProcess();
extern char *optarg;
extern int opterr;


int main( int argc, char * argv[] ) {

	int numberOfProcess = getNumberOfProcess( argc, argv );

	for ( int i = 0; i < numberOfProcess; i++ ) {
		if ( fork() == 0 ) {
			ChildProcess();
			break; // Not to do fork() from the child
		} else if ( i == numberOfProcess - 1 ) { // The last child has been created
			ParentProcess();
		}
	}

	return 0;
}


void  ChildProcess() {
	printf("*** Child process is done ***\n");
}


void  ParentProcess() {
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
