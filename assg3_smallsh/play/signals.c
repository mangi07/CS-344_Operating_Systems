#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <signal.h>


pid_t spawnpid = -5;
pid_t w_pid = -5;
int status = -5;
int exit_status = -5;
int isBackground = 0;

// may not need to use this handler
/*
void catchint( int signo ) {
	
	if ( spawnpid == 0 ) { // child
		printf( "child caught signal and is exiting\n\n" );
		exit( signo ); // or send the signal corresponding to signo, programatically
	} else if ( WIFSIGNALED( status ) ) { // parent
		printf( "parent caught signal and is ignoring it\n\n" );
		// relay this signal to the child
		kill( w_pid, signo );
	}
	
	//printf( "Caught an interrupt: %d\n", signo );
}
*/

// TODO need to learn how to make parent ignore signal while child uses catchint!!
int main(int argc, char **argv) {


	struct sigaction parent_act, child_act;

	parent_act.sa_handler = SIG_IGN;

	//child_act.sa_handler = catchint;
	//child_act.sa_flags = 0;
	//sigfillset( &( child_act.sa_mask ) );

	sigaction( SIGINT, &parent_act, NULL );

	printf( "In parent!\n" );	
	spawnpid = fork();

	switch (spawnpid)
	{
		case -1:
			perror("Hull Breach!");
			exit(1);
			break;
		case 0: // child, where command is executed, whether foreground or background
			printf( "In child!\n" );
			
			if ( ! isBackground ) sigaction( SIGINT, &child_act, NULL );
			
			//sleep( 5 );
			execlp("sleep", "sleep", "5", NULL);
			// If we got here, there was an error with exec
			perror( "sleep" );
			return( 1 );
			break;
		default: // parent
			printf("In parent, switch default!\n");
			w_pid = waitpid( spawnpid, &status, 0 );
			if (WIFEXITED(status)) {
				exit_status = WEXITSTATUS(status);
				printf( "exit value %d\n", exit_status );
			} else if ( WIFSIGNALED( status ) ) {
				// if there was a signal caught (should be caught elsewhere), print out the signal instead
				printf("terminated by signal %d\n", status );
			}
			break;
	}
	printf( "We're back in the parent, sleeping...\n");
	sleep( 5 );

	exit( 0 );

}




