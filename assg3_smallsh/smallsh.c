/************************************
 * Author: Ben R. Olson
 * Assignment: CS 344 Operating Systems Assg 3 Small sh
 * *********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct command {
	char* name;
	char** args;
	int argc;
	int out_redir;
	char* out_file;
	int in_redir;
	char* in_file;
	int background;
};

int main(int argc, char **argv) {

	printf( ":\n" );

	const int MAXARGS = 512;
	
	// Initialize command struct
	struct command C;
	memset( &C, 0, sizeof( C ) );
	C.args = malloc( MAXARGS * sizeof( char ) );
	C.argc = 0;

	// Get command input into a buffer
	char buffer[2048] = {0};

	// Get args and fill argument struct
	if ( argc >= 2 ) { // should be the command itself
		C.name = argv[1];
	}
	int i = 0;
	int offset = 2;
	while ( i < MAXARGS - offset && 
		i < argc - offset &&
		strcmp( argv[i], "<" ) != 0 &&
		strcmp( argv[i], ">" ) != 0 &&
		strcmp( argv[i], "&" ) != 0 ) {
		// Do stuff here
		C.args[i] = argv[i + offset];
		printf( "C.args[%d] = %s\n", i, argv[i] ); // debug
		C.argc++;
		i++;
	}
	
	// this is debug stuff	
	if ( C.name != NULL ) {
		printf( "%s\n", C.name );
	}
	/*
	int j;
	for ( j = 0; j < C.argc; j++ ) {
		printf( "%s\n", argv[j] );
	}
	*/

	free( C.args );
	
	// exit cd and status

	exit( 0 );

}

