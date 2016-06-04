
/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

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


void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	curr_client_idx = 0;
	int portno = atoi( argv[1] );
	int sockfd = init_socket_fd( portno );
	// loop accept in start_clients
	start_clients( sockfd, clients, curr_client_idx );
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
	// accept happens here
	clients[curr].fd = new_client_fd( sockfd );
	// fork and do the encryption in the child process
	fork_child( clients, curr );
}

int new_client_fd( int sockfd ) {
	int newsockfd;
	socklen_t clilen;
	struct sockaddr_in cli_addr;

	clilen = sizeof(cli_addr);
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
			assign_new_portno( clients, curr );
			// let client know about new port
			send_new_portno( *c );
			close( c->fd );

			// set up new socket on server with the new port number
			int temp_fd = init_socket_fd( c->portno );
			c->fd = new_client_fd( temp_fd );
			// new handshake with client on this new connection
			verify_new_connection( c->fd );
			printf( "made it all the way through on server!\n" );
			close( c->fd );
			exit( 0 );
		default: // parent
			// waitpid is called on each of the client pids, in a different method 
			b_ret = waitpid( spawnpid, NULL, WNOHANG );
			if ( b_ret == -1 ) {
				printf( "Could not wait for child in switch defalut!\n" );
				exit( 1 );
			}
			printf( "child pid is %d\n", spawnpid ); // debug
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

void communicate( int sockfd ) {
	// going to need to fork and start up on new port for this read/write stuff
	// as specified in "Concurrency Implications" of assignment spec file
	char buffer[100000];
	bzero(buffer,100000);
	int n = read(sockfd,buffer,99999);
	if (n < 0) error("ERROR reading from socket");
	printf("Server says: here is the message: %s\n",buffer);
	n = write(sockfd,"Server says: I got your message",100);
	if (n < 0) error("ERROR writing to socket");
}



