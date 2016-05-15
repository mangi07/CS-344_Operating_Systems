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
#include <linux/limits.h>
#include <sys/wait.h>

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

char buffer[2048] = {0};
void getCommand( Command *C ) {

	printf( ": " );

	// Initialize command struct
	memset( &(*C), 0, sizeof( (*C) ) );
	C->args = malloc( MAXARGS * sizeof( char ) );
	C->argc = 0;

	// Get command input into a buffer
	memset( &buffer, 0, sizeof(char) * 2048 );
	// See if this works with the script to read in one line at a time
	if ( fgets( buffer, sizeof(buffer), stdin ) == NULL ) {
		perror( "fgets" );
		fflush( NULL );
		exit( 1 );
	}
	int len = strlen(buffer);
	if ( len > 0 && buffer[ len - 1 ] == '\n' ) {
		buffer[ len - 1 ] = '\0';
	}
	// check whether this command is to be a background process
	len = strlen(buffer);
	if ( len >= 1 && buffer[ len - 1 ] == '&' ) {
		C->isBackground = 1;
		buffer[ len - 1 ] = '\0';
	}

	char *next_word = strtok( buffer, " " );
	char *command = NULL;
	if ( next_word == NULL || next_word[0] == '#' ) {
		// User is commenting, so do nothing
		return;
	} else {
		C->name = next_word;
		C->args[0] = next_word;
	}
	int args_count = 1;
	while ( next_word != NULL && args_count < MAXARGS &&
			next_word[0] != '<' &&
			next_word[0] != '>' ) {
		next_word = strtok( NULL, " " );
		C->args[ args_count ] = next_word;
		if ( C->args[ args_count ] != NULL ) args_count++;
		C->argc = args_count;
	}
	C->args[ args_count ] == NULL;
	args_count++;
	if ( next_word != NULL && next_word == "<" ) {
		C->redir = IN_FILE;
	} else if ( next_word != NULL && next_word == ">" ) {
		C->redir = OUT_FILE;
	}

}


int main(int argc, char **argv) {

	Command C;
	Command *C_ptr = &C;
	int status = -5;
	// declare array of background pids here

	int shouldRepeat = 1;
	while ( shouldRepeat ) {
		// TODO check any background processes here: refer to lecture 9, p 21, 4th bullet point
		getCommand( C_ptr );
		// exit implemented here
		if ( C.name != NULL && strcmp( C.name, "exit" ) == 0 ) {
			shouldRepeat = 0;
			// Note: You must kill all processes and jobs started by this shell here!!
		} else if ( C.name != NULL && strcmp( C.name, "cd" ) == 0 ) { 
			// cd implemented here
			char *home = getenv( "HOME" );
			// C.args[0] should be the command itself
			if ( C.args[1] == NULL && home != NULL ) {
				chdir( home );
			} else if ( C.args[1] != NULL ) {
				char * dir = malloc( strlen( C.args[1] + 1 ) * sizeof( char ) );
				strcpy( dir, C.args[1] );
				int ret_val = chdir( dir );
				if ( ret_val == -1 ) {
					char err_cmd[2048] = {0};
					sprintf( err_cmd, ": cd: %s", dir );
					perror( err_cmd );	
				}
				free( dir );

				// pwd to see if this worked
				/*
				   dir = malloc( PATH_MAX );
				   if ( ! getcwd( dir, PATH_MAX ) ) {
				   perror( ": getcwd" );
				   } else {
				   printf( "%s\n", dir );
				   }
				   free( dir );
				   */
			}
		} else if ( C.name != NULL && strcmp( C.name, "status" ) == 0 ) { 
			// status implemented here
			// print out the exit status OR terminating signal of the last FOREGROUND process
			if ( status >= 0 ) { // status has been set by the last FOREGROUND process
				// printf( "exit value %d\n", status );

				// TODO or use this example from lecture 9, p 20
				if (WIFEXITED(status)) {
					int exitstatus = WEXITSTATUS(status);
					printf("exit value %d\n", exitstatus);
				} else if ( WIFSIGNALED( status ) ) {
					// if there was a signal caught (should be caught elsewhere), print out the signal instead
					printf("Child terminated by a signal\n");
				}
			}	
		} else if ( C.name == NULL ) {
			// do nothing, user commented
		} else { 
			// all other commands executed here
			// modified from lectures
			pid_t spawnpid = -5;
			pid_t w_pid = -5;

			int ten = 10;
			printf( "This should only show if we are in the parent!\n" );

			printf( "C.name right before the fork: %s\n", C.name );
			spawnpid = fork();

			switch (spawnpid)
			{
				case -1:
					perror("Hull Breach!");
					exit(1);
					break;
				case 0: // child, where command is executed, whether foreground or background
					// must be a command other than cd, status, or exit
					printf( "I am the child!\n" );
					// change execlp to execv or execvp, as shown in lecture 9, p 34,
					printf( "C.name in the child: %s\n", C.name );
					char *args[512] = {0};
					int i;
					for ( i = 0; i < C.argc; ++i ) {
						args[i] = C.args[i];
					}
					execvp(C.name, args);
					// If we got here, there was an error with exec
					// so implement according to spec section "Command Execution" 2nd paragraph
					perror( C.name );
					return( 1 );
					break;
				default: // parent
					ten = ten - 1;
					printf("I am the parent! ten = %d\n", ten);
					printf( "C.name in the parent: %s\n", C.name );
					if ( ! C.isBackground ) { 
						// The given command is a foreground process, so wait here 
						w_pid = waitpid( spawnpid, &status, 0 );
						// check for failed wait?? see lecture 9, p 20
					} else { 
						// The given command must be treated as a background process,
						// so waitpid should be called right before each iteration
						// of getCommand()
						// Also, child pid needs to be added to array,
						// as explained at bottom of spec p 1
					}
					break;
			}
			//printf("This should only be executed in the parent!\n");

		}
	}


	free( C.args );


	exit( 0 );

}

