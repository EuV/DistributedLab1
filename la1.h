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
#define WRITE 1
#define READ 0

typedef struct {
    local_id localId;
    int total;
} Process;

int getNumberOfProcess( int argc, char * const argv[] );
void createFullyConnectedTopology( const int );
void makePipeLog( const int );
void makeChildren( const int );

void childProcess( const Process * const );
void parentProcess( const Process * const );

void closeUnusedPipes( const Process * const );
void fillMessage( Message *, const MessageType, const local_id );
void makeLogging( const char * const, const size_t );
void receiveAll( void * self, const MessageType, const int );
void closeOtherPipes( const Process * const );

void waitForChildren();

int Pipes[ MAX_PROCESS_ID + 1 ][ MAX_PROCESS_ID + 1][ 2 ];
int EventsLog;
int PipesLog;
char LogBuf[ BUF_SIZE ];
