
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
void fork_child( struct client );
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

	//assign_new_portno( clients, curr_client_idx );

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

// fork happens in this method
// sockfd is the parent socket fd
void start_clients( int sockfd, struct client clients[], int curr_client_idx ) {
	// accept happens here
	clients[curr_client_idx].fd = new_client_fd( sockfd );
	// agree on new port, now
	assign_new_portno( clients, curr_client_idx );
	send_new_portno( clients[curr_client_idx] );
	// fork and do the encryption in the child process
	fork_child( clients[curr_client_idx] );
}

// assigns (modifies) pid, fd, and portno of struct passed in
void fork_child( struct client c ) {
	// modified from lectures
	pid_t spawnpid = -5;
	int b_ret;
	spawnpid = fork();
	switch (spawnpid)
	{
		case -1:
			perror("Hull Breach!");
			exit(1);
			break;
		case 0: // child
			// do stuff
			// send_new_portno( c );
			communicate( c.fd ); // this will happen after a fork
			close( c.fd );
		default: // parent
			// waitpid is called on each of the client pids, in a different method 
			b_ret = waitpid( spawnpid, NULL, WNOHANG );
			if ( b_ret == -1 ) {
				printf( "Could not wait for child in switch defalut!\n" );
				exit( 1 );
			}
			printf( "child pid is %d\n", spawnpid ); // debug
			c.pid = spawnpid;
			break;
	}
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


/* Child Methods */

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

// test
void send_new_portno( struct client c ) {
	char buffer[10];
	bzero( buffer, 10 );
	int sockfd = c.fd;
	int n = write( sockfd, buffer, 10 );
	if (n < 0) error("ERROR writing to socket");
}




