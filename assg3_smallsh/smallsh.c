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
#include <sys/stat.h>
#include <fcntl.h>

struct command {
	char* name;
	char** args;
	int argc;
	int redir; // if assigned, should be either OUT_FILE or IN_FILE
	char* file;
	int isBackground;
};
typedef struct command Command;

struct b_pid {
	int pid;
	struct b_pid *next;
};
typedef struct b_pid B_pid;


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
	C->redir = 0;

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
	
	// get command name if user is not commenting
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
	
	// check for regular args to the command
	next_word = strtok( NULL, " " );
	while ( next_word != NULL && strcmp( next_word, "<" ) != 0 
				  && strcmp( next_word, ">" ) != 0 
				  && strcmp( next_word, "&" ) != 0 ) {
		C->args[ args_count ] = next_word; // okay if NULL is assigned	
		args_count++;
		next_word = strtok( NULL, " " );
	}
	// last arg of command must be NULL for execvp which will be called later
	C->args[ args_count + 1 ] = NULL;
	C->argc = args_count + 1;

	// check for redirection and get file name
	if ( next_word != NULL && strcmp( next_word, ">" ) == 0 ) {
		 C->redir = OUT_FILE;
		next_word = strtok( NULL, " " );
		if (next_word != NULL ) { 
			C->file = next_word;
			next_word = strtok( NULL, " " );
		} else {
			printf( "command syntax error: expected file name!\n" );
			exit( 1 );
		}
	} else if ( next_word != NULL && strcmp( next_word, "<" ) == 0 ) {
		C->redir = IN_FILE;
		next_word = strtok( NULL, " " );
		if ( next_word != NULL ) {
			C->file = next_word;
			next_word = strtok( NULL, " " );
		} else {
			printf( "command syntax error: expected file name!\n" );
			exit( 1 );
		}
	}

	// check for & and assign isBackground if present
	if ( next_word != NULL && strcmp( next_word, "&" ) == 0 ) C->isBackground = 1;
	next_word = strtok( NULL, " " );

	// maybe delete the rest of this function...
	/*
	while ( next_word != NULL && args_count < MAXARGS ) {
		next_word = strtok( NULL, " " );
		if ( next_word == NULL ) break;
		if ( next_word == ">" ) {
			C->redir = OUT_FILE;
			isArg = 0;
		} else if ( next_word == "<" ) {
			C->redir = IN_FILE;
			isArg = 0;
		}
		if ( isArg ) {
			C->args[ args_count ] = next_word;
			args_count++;
		} else { // it must be a file
			C->file = next_word;
		}
	}
	C->argc = args_count;
	C->args[ args_count ] == NULL;

	// check whether this command is to be a background process
	if ( args_count > 1 && strcmp( C->args[ args_count-1 ], "&" ) == 0 ) {
		C->isBackground = 1;
		C->args[ args_count-1 ] = NULL;
		C->argc--;
	}
	*/
	// debug
	/*
	int debug_count;
	printf( "C->name: %s\n", C->name );
	for ( debug_count = 0; debug_count < C->argc; debug_count++ ) {
		printf( "C->args[%d]: %s\n", debug_count, C->args[debug_count] );
	}
	printf( "C->argc: %d\n", C->argc );
	printf( "C->file: %s\n", C->file );
	printf( "C->redir: %d\n", C->redir );
	printf( "C->isBackground: %d\n", C->isBackground );
	*/
}


int main(int argc, char **argv) {

	// initialize a linked list for background pids
	// as LIFO, inserting each new element at the head
	B_pid *b_pids_head = 0;

	/* signals */
	struct sigaction parent_act, child_act;
	parent_act.sa_handler = SIG_IGN;
	sigaction( SIGINT, &parent_act, NULL );

	Command C;
	Command *C_ptr = &C;
	int status = -5;
	int exit_status = -5;

	int shouldRepeat = 1;
	while ( shouldRepeat ) {
		// print out status(es) of background processes here if exited
		B_pid *curr_b_pid = b_pids_head;
		B_pid *prev_b_pid = b_pids_head;
		//int inf_loop_safeguard = 10;
		while ( curr_b_pid != 0 /*&& inf_loop_safeguard > 0*/ ) {
			int b_status = -5;
			int b_w = waitpid( curr_b_pid->pid, &b_status, WNOHANG );
			// debug
			//printf( "checking background processes at top, pid: %d, b_status: %d\n", curr_b_pid->pid, b_status );
			if ( b_w == -1 ) { perror("waitpid"); exit( 1 ); }
			int isDone = 0;
			if ( b_w == 0 ) {
				// debug
				//printf( "process %d still going\n", curr_b_pid->pid );
			} else if ( WIFEXITED( b_status ) ) {
				int b_ret_status = WEXITSTATUS( b_status );
				printf( "background pid %d is done: exit value %d\n", curr_b_pid->pid, b_ret_status );
				isDone = 1;
			} else if ( WIFSIGNALED( b_status ) ) {
				// if there was a signal caught
				printf( "background pid %d is done: terminated by signal %d\n", curr_b_pid->pid, b_status );
				isDone = 1;
			}
			if ( isDone ) {	
				// remove this background pid from the linked list
				if ( b_pids_head->next == 0 ) { // only one background process
					free( b_pids_head );
					b_pids_head = 0;
					curr_b_pid = 0;
					prev_b_pid = 0;
					// debug
					//printf( "removed head\n" );
				} else if ( prev_b_pid == curr_b_pid ) { // two background processes, where curr is head
					b_pids_head = curr_b_pid->next;
					prev_b_pid = curr_b_pid->next;
					free( curr_b_pid );
					curr_b_pid = b_pids_head;
					// debug
					//printf( "removed head, but one more background process remains\n" );
				} else { // at least two background processes, where curr is ahead of prev by one
					prev_b_pid->next = curr_b_pid->next;
					free( curr_b_pid );
					curr_b_pid = prev_b_pid->next;
					// debug
					//printf( "removed curr, where at least two backgroud processes exits\n" );
				}

				// debug
				//printf( "debug curr_b_pid: %d\n", curr_b_pid );
				//printf( "debug prev_b_pid: %d\n", prev_b_pid );
				//printf( "debug b_pids_head: %d\n", b_pids_head );
			} else { // move the pid pointers
				prev_b_pid = curr_b_pid;
				curr_b_pid = curr_b_pid->next;
			}
			// debug
			//printf( "waiting: inf_loop_safeguard: %d\n", inf_loop_safeguard );
			//inf_loop_safeguard--;
		}

		getCommand( C_ptr );

		// exit implemented here
		if ( C.name != NULL && strcmp( C.name, "exit" ) == 0 ) {
			shouldRepeat = 0;
			// Kill all processes and jobs started by this shell here!!
			// Go through linked list and kill all running background processes with kill()
			while (b_pids_head != 0) {
				kill( b_pids_head->pid, SIGTERM );
				B_pid *temp = b_pids_head->next;
				free( b_pids_head );
				b_pids_head = temp;
			}
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
				// TODO or use this example from lecture 9, p 20
				if (WIFEXITED(status)) {
					int exitstatus = WEXITSTATUS(status);
					printf("exit value %d\n", exitstatus);
				} else if ( WIFSIGNALED( status ) ) {
					// if there was a signal caught (should be caught elsewhere), print out the signal instead
					printf("terminated by signal %d\n", status);
				}
			}	
		} else if ( C.name == NULL ) {
			// do nothing, user commented
		} else { 
			// all other commands executed here
			// modified from lectures
			pid_t spawnpid = -5;
			pid_t w_pid = -5;

			// debug
			//printf( "In the parent right before the fork\n" );

			spawnpid = fork();

			switch (spawnpid)
			{
				case -1:
					perror("Hull Breach!");
					exit(1);
					break;
				case 0: // child, where command is executed, whether foreground or background
					// must be a command other than cd, status, or exit
					// debug
					//printf( "I am the child!\n" );

					/* signals */
					if ( ! C.isBackground ) sigaction( SIGINT, &child_act, NULL );

					char *args[512] = {0};
					int i;
					for ( i = 0; i < C.argc; ++i ) {
						args[i] = C.args[i];
					}
					// TODO In the case of background processes, 
					// redirect output to dev/null/ if it's not being directed to a file
					if ( C.isBackground ) {
						int devNull_fd = open( "/dev/null", O_RDWR );
						if ( devNull_fd == -1 ) {
							// file error, so don't run the rest of command C
							perror( "open" );
							exit( 1 );
						}
						int devNull_fd2 = dup2( devNull_fd, 1 );
						if ( devNull_fd2 == -1 ) {
							// file error, so don't run the rest of command C
							perror( "dup2" );
							exit( 1 );
						}
						int devNull_fd3 = dup2( devNull_fd, 0 );
						if ( devNull_fd3 == -1 ) {
							// file error, so don't run the rest of command C
							perror( "dup2" );
							exit( 1 );
						}
						//close( devNull_fd );
						//close( devNull_fd2 );
					}
					// if condition met, redirect stdout to file given in command
					else if ( C.redir == OUT_FILE && C.file != NULL ) {
						// check that the arguments meet the specs
						// TODO also, consider closing this file at the end of the outer while loop
						int fd = open( C.file, O_WRONLY|O_CREAT|O_TRUNC, 0644 );
						if ( fd == -1 ) {
							// file error, so don't run the rest of command C
							perror( "open" );
							exit( 1 );
						}
						int fd2 = dup2( fd, 1 );
						if ( fd2 == -1 ) {
							// file error, so don't run the rest of command C
							perror( "dup2" );
							exit( 1 );
						}
						//close( fd );
						//close( fd2 );
					}
					// if condition met, redirect stdin to file given in command, 
					// to read in from file that which is input to execvp command
					else if ( C.redir == IN_FILE && C.file != NULL ) {
						// check that the arguments meet the specs
						// also, consider closing this file at the end of the outer while loop
						int fd = open( C.file, O_RDONLY );
						if ( fd == -1 ) {
							// file error, so don't run the rest of command C
							perror( "open" );
							exit( 1 );
						}
						int fd2 = dup2( fd, 0 );
						if ( fd2 == -1 ) {
							// file error, so don't run the rest of command C
							perror( "dup2" );
							exit( 1 );
						}
						//close( fd );
						//close( fd2 );
					}
					execvp(C.name, args);
					// If we got here, there was an error with exec
					perror( C.name );
					return( 1 );
					break;
				default: // parent
					// debug
					//printf("In parent, switch default!\n");
					// close-on-exec needed? ie: fctnl(fd, F_SETFD, FD_CLOEXEC) ?
					if ( ! C.isBackground ) { 
						// The given command is a foreground process, so wait here 
						w_pid = waitpid( spawnpid, &status, 0 );
						// check here for interrupt sent to parent (note: duplicated from somewhere above)
						if ( status >= 0 ) { // status has been set by the last FOREGROUND process
							// TODO or use this example from lecture 9, p 20
							if ( WIFSIGNALED( status ) ) {
								// if there was a signal caught (should be caught elsewhere), 
								// print out the signal instead
								printf("terminated by signal %d\n", status);
							}
						}
					} else { 
						// The given command must be treated as a background process,
						// so waitpid is called on each of the b_pid linked list, 
						// at the beginning of this while loop, right before getCommand() is called again
						int b_ret = waitpid( spawnpid, NULL, WNOHANG );
						if ( b_ret == -1 ) {
							printf( "Could not wait for child in switch defalut!\n" );
							exit( 1 );
						}
						printf( "background pid is %d\n", spawnpid );

						// insert new background process ID at head of b_pids linked list
						B_pid *new_b_pid = malloc( sizeof( B_pid ) );
						new_b_pid->pid = spawnpid;
						new_b_pid->next = b_pids_head;
						b_pids_head = new_b_pid;
					}



					break;
			}

		}


	}


	free( C.args );
	while (b_pids_head != 0) {
		B_pid *temp = b_pids_head->next;
		free( b_pids_head );
		b_pids_head = temp;
	}


	exit( 0 );

}

