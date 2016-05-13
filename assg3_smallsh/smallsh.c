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
	int redir; // if assigned, should be either OUT_FILE or IN_FILE
	char* file;
	int isBackground;
};
typedef struct command Command;

static const int OUT_FILE = 1;
static const int IN_FILE = -1;
static const int MAXARGS = 512;
static const int MAXLEN = 2048;

void getCommand( Command *C ) {

	printf( ": " );

	// Initialize command struct
	memset( &(*C), 0, sizeof( (*C) ) );
	C->args = malloc( MAXARGS * sizeof( char ) );
	C->argc = 0;

	// Get command input into a buffer
	char buffer[2048] = {0};
	if ( fgets( buffer, sizeof(buffer), stdin ) == NULL ) {
		perror( "fgets" );
		exit( 1 );
	}
	int len = strlen(buffer);
	if ( len > 0 && buffer[ len - 1 ] == '\n' ) {
		buffer[ len - 1 ] = '\0';
	}
	// check whether this command is a background process
	len = strlen(buffer);
	if ( len >= 1 && buffer[ len - 1 ] == '&' ) {
		C->isBackground = 1;
		buffer[ len - 1 ] = '\0';
	}
	
	char *next_word = strtok( buffer, " " );
	char *command = NULL;
	if ( next_word == NULL || next_word[0] == '#' ) {
		// return from function yet to be extracted
		// and indicate somehow that no command was given
	} else {
		C->name = next_word;
	}
	int args_count = 0;
	while ( next_word != NULL && args_count < MAXARGS &&
		next_word[0] != '<' &&
		next_word[0] != '>' ) {
		// printf( "%s\n", next_word );
		next_word = strtok( NULL, " " );
		C->args[ args_count ] = next_word;
		if ( C->args[ args_count ] != NULL ) args_count++;
		C->argc = args_count;
	}
	if ( next_word != NULL && next_word == "<" ) {
		C->redir = IN_FILE;
	} else if ( next_word != NULL && next_word == ">" ) {
		C->redir = OUT_FILE;
	}

}


int main(int argc, char **argv) {
	
	Command C;
	Command *C_ptr = &C;

	int shouldRepeat = 1;
	while ( shouldRepeat ) {
		getCommand( C_ptr );
		if ( C.name != NULL && strcmp( C.name, "exit" ) == 0 ) {
			shouldRepeat = 0;
			// Note: You must kill all processes and jobs started by this shell here!!
		} else if ( C.name != NULL && strcmp( C.name, "cd" ) == 0 ) {
			char *home = getenv( "HOME" );
			if ( C.args[0] == NULL && home != NULL ) {
				if ( home != NULL ) {
					chdir( home );
				}
			} else if ( C.args[0] != NULL ) {
				char * dir = malloc( strlen( C.args[0] + 1 ) * sizeof( char ) );
				strcpy( dir, C.args[0] );
				int ret_val = chdir( dir );
				if ( ret_val == -1 ) {
					char err_cmd[2048] = {0};
					sprintf( err_cmd, ": cd: %s", dir );
					perror( err_cmd );	
				}
				free( dir );
			}
		}
	}


	free( C.args );
	
	// exit cd and status

	exit( 0 );

}

