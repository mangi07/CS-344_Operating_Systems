
/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "enc.h"


struct client {
	int pid;
	int portno;
	int fd;
};
struct client clients[5];
int curr_client_idx;

int init_socket_fd( int portno );
void start_clients( int sockfd, struct client clients[], int curr_client_idx );
void fork_child( struct client clients[], int curr );
void verify_new_connection( int fd );
int new_client_fd( int sockfd );
// need method to check clients pids for exit to avoid zombies
void set_curr_client( int *curr_client_idx );
void assign_new_portno( struct client clients[], int curr_idx );
int rand_range();
void send_new_portno( struct client ); // implement
// after fork, server starts listending on this new port
// then curr client sends its first request on this new port
// (is curr client idx shared between processes??)
void communicate( int sockfd );
void manage_clients( struct client clients[], int *curr_client );
void show_clients( struct client clients[] );
void read_all( char *buffer, int buff_size, int fd );
void write_all( char *buffer, int buff_size, int fd );

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	signal( SIGCHLD, SIG_IGN );
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	curr_client_idx = 0;
	int portno = atoi( argv[1] );
	int sockfd = init_socket_fd( portno );
	// loop accept in start_clients
	while ( 1 ) { // test
		start_clients( sockfd, clients, curr_client_idx );
		//printf( "\n\nserver trace 1\n\n" );
		manage_clients( clients, &curr_client_idx );
		//printf( "\n\nserver trace 2\n\n" );
		show_clients( clients );
		//printf( "\n\nserver trace 3\n\n" );
	} // newest
	close(sockfd);

	return 0; 
}


int init_socket_fd( int portno ) {
	int sockfd;
	struct sockaddr_in serv_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	listen(sockfd,5);

	return sockfd;
}

// sockfd is the parent socket fd
void start_clients( int sockfd, struct client clients[], int curr ) {
	//printf( "\n\nserver trace start_clients1\n\n" );
	// accept happens here
	clients[curr].fd = new_client_fd( sockfd );
	//printf( "\n\nserver trace start_clients2\n\n" );
	// fork and do the encryption in the child process
	fork_child( clients, curr );
	//printf( "\n\nserver trace start_clients3\n\n" );
}

int newsockfd;
socklen_t clilen;
struct sockaddr_in cli_addr;
int new_client_fd( int sockfd ) {
	//int newsockfd;
	//socklen_t clilen;
	//struct sockaddr_in cli_addr;

	clilen = sizeof(cli_addr);
	// trace segfault here??
	//printf( "\n\nserver trace new_client_fd pid = %d\n\n", getpid() );
	newsockfd = accept(sockfd, 
			(struct sockaddr *) &cli_addr, 
			&clilen);
	if (newsockfd < 0) 
		error("ERROR on accept");

	return newsockfd;
}

// assigns (modifies) pid, fd, and portno of struct passed in
void fork_child( struct client clients[], int curr ) {
	// modified from lectures
	pid_t spawnpid = -5;
	int b_ret;
	struct client *c = &clients[curr];

	assign_new_portno( clients, curr );

	spawnpid = fork();
	switch (spawnpid)
	{
		case -1:
			perror("Hull Breach!");
			exit(1);
			break;
		case 0: // child
			verify_new_connection( c->fd ); // this is actually on the original socket
			// set up new port connection on server
			//assign_new_portno( clients, curr );
			// let client know about new port
			send_new_portno( *c );
			close( c->fd );

			// set up new socket on server with the new port number
			int temp_fd = init_socket_fd( c->portno );
			printf( "\n\ntrace case 0: child pid = %d\n\n", getpid() );
			c->fd = new_client_fd( temp_fd );
			// new handshake with client on this new connection
			verify_new_connection( c->fd );
			communicate( c->fd );
			printf( "****************made it all the way through on server!\n" );
			close( c->fd );
			exit( 0 );
		default: // parent
			// waitpid is called on each of the client pids, in a different method 
			b_ret = waitpid( spawnpid, NULL, WNOHANG );
			if ( b_ret == -1 ) {
				printf( "Could not wait for child in switch defalut!\n" );
				exit( 1 );
			}
			//printf( "child pid is %d\n", spawnpid ); // debug
			c->pid = spawnpid;
			break;
	}
}

void assign_new_portno( struct client clients[], int curr_idx ) {
	int portno;
	int i;
	do {
		portno = rand_range();
		// check if portno is used
		for ( i = 0; i < 5; ++i ) {
			if ( clients[i].portno == portno )
				break;
		}
	} while ( i < 5 ); // if i == 5 at this point, the portno is unique
	// assign unique portno to the current client
	clients[curr_idx].portno = portno;
}

int rand_range() {
	srand(time(0));
	int min = 50000;
	int max = 100000;
	int num = ( rand() % (max - (min - 1)) ) + min;
	return num;
}

void send_new_portno( struct client c ) {
	char buffer[10];
	bzero( buffer, 10 );
	// convert port number to fill buffer
	snprintf( buffer, sizeof(buffer), "%d\0", c.portno );	
	printf( "On server in send_new_portno: buffer sent: %s\n", buffer );
	int n = write( c.fd, buffer, 9  );
	if (n < 0) error("ERROR writing to socket on server line 173");
	// verify with client that all was read
	bzero( buffer, 10 );
	n = read( c.fd, buffer, 9 );
	if ( n < 0 ) error( "Server could not verify client read in send_new_portno.\n" );
}

void verify_new_connection( int fd ) {
	char buffer[100];
	bzero(buffer,100);
	int n = read(fd,buffer,99);
	if (n < 0) error("Server could not read verification from client.\n");
	printf("Server says in verify_new_connection: %s\n",buffer);

	n = write( fd, "Server verify", 100 );
	if ( n < 0 ) error("Server could not write verification to client.\n");
}

// the first client struct with its portno set to 0 is open for use
// and will be chosen
void set_curr_client( int *curr_client_idx ) {
	int i;
	for ( i = 0; i < 5; ++i ) {
		if ( clients[i].portno == 0 ) {
			*curr_client_idx = i;
			return;
		}
	}
}

// should happen inside the fork
void communicate( int sockfd ) {
	// receive message from client, to be encrypted
	char text_buffer[100000];
	bzero(text_buffer,100000);
	read_all( text_buffer, 100000, sockfd );
	printf("SERVER: here is the message (plaintext): %s\n", text_buffer);
	// receive key from client, used for enctryption
	char key_buffer[100000];
	bzero(key_buffer,100000);
	read_all( key_buffer, 100000, sockfd );
	printf("SERVER: here is the message (key): %s\n", key_buffer);
	
	// encrypt message and send to client
	enc( text_buffer, key_buffer ); // method included from enc.h
	write_all( text_buffer, 100000, sockfd ); // received text will be translated before this send
}

// fills the buffer in chunks
void read_all( char *buffer, int buff_size, int fd ) {
	int n;
	int tally = 0;
	char temp_buffer[1000];
	while( (n = read( fd, temp_buffer, 1000 )) > 0 && tally <= buff_size) {
		// append current buffer to destination buffer
		strcat( buffer, temp_buffer );
		tally += n;
		//printf( "SERVER: in read_all, return value of read: %d and tally = %d\n", n, tally );
		printf( "SERVER: in read_all, buffer: %s\n", buffer );
		if ( tally >= buff_size ) break;
	}
	//printf( "\n\nSERVER: trace1 read_all buffer: %s\n\n", buffer );
	if (n < 0) { 
		error("ERROR reading from socket, in read_all");
	}
}

void write_all( char *buffer, int buff_size, int fd ) {
	int n;
	while( (n = write( fd, buffer, buff_size )) < buff_size ) {
		printf( "SERVER: in write_all, return value of write: %d\n", n );
	}
	//printf( "\n\nSERVER: trace1 write_all\n\n" );
	if (n < 0) { 
		error("ERROR writing to socket, in write_all");
	}
}

void manage_clients( struct client clients[], int *curr_client ) {
	// check pids
	//   set all fields back to zero on finished processes
	int i;
	for ( i = 0; i < 5; ++i ) {
		int b_status = -5;
		int b_w;
		if ( clients[i].pid != 0 ) b_w = waitpid( clients[i].pid, &b_status, WNOHANG );
		if ( b_w == -1 ) { perror("waitpid"); exit( 1 ); }
		int isDone = 0;
		if ( b_w == 0 ) {
			// do nothing
		} else if ( WIFEXITED( b_status ) ) {
			int b_ret_status = WEXITSTATUS( b_status );
			printf( "\nbackground pid %d is done: exit value %d\n\n", clients[i].pid, b_ret_status );
			isDone = 1;
		} else if ( WIFSIGNALED( b_status ) ) {
			// if there was a signal caught
			printf( "\nbackground pid %d is done: terminated by signal  %d\n\n", clients[i].pid, b_status );
			isDone = 1;
		}
		if ( isDone ) {
			clients[i].pid = 0;
			clients[i].portno = 0;
			clients[i].fd = 0;
		}
	}
	// select an open client struct to use
	int j;
	for ( j = 0; j < 5; ++j ) {
		if ( clients[j].pid = 0 ) *curr_client = j;
	}
}

// for debugging purposes
void show_clients( struct client clients[] ) {
	printf( "Executing show_clients\n" );
	int i;
	for ( i = 0; i < 5; ++i ) {
		printf( "SHOWING CLIENTS: client: %d:\tpid:\t%d\tportno:\t%d\tfd:\t%d\n",
			i,
			clients[i].pid,
			clients[i].portno,
			clients[i].fd );
	}
}




