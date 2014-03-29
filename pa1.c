#include "pa1.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int getNumberOfProcess( int argc, char * const argv[] );
int getopt( int argc, char * const argv[], const char * optstring );
extern char *optarg;
extern int opterr;

const int NUMBER_OF_PROCESS = 6;


int main( int argc, char * argv[] ) {

	int numberOfProcess = getNumberOfProcess( argc, argv );

	printf( "Number of child process: %d\n", numberOfProcess );

	return 0;
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
