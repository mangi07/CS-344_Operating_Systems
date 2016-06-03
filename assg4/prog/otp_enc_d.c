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

int init_socket( int portno );
int new_client_fd( int sockfd );
void communicate( int sockfd );
int find_new_portno( struct client clients[] );
int rand_range();
void send_new_portno( int sockfd );

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

	int portno = atoi( argv[1] );
	int sockfd = init_socket( portno );
	int new_sockfd = new_client_fd( sockfd );
	communicate( new_sockfd ); // this will happen after a fork
	close(sockfd);
	close(new_sockfd);

	int newportno = find_new_portno( clients );
	
	return 0; 
}


int find_new_portno( struct client clients[] ) {
	int i;
	for ( i = 0; i < 5; ++i ) {
		if ( clients[i].portno == 0 ) {
			clients[i].portno = rand_range();
		}
	}
	return 0;
}
int rand_range() {
	srand(time(0));
	int min = 50000;
	int max = 100000;
	int num = ( rand() % (max - (min - 1)) ) + min;
	return num;
}
void send_new_portno( int sockfd );



int init_socket( int portno ) {
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




