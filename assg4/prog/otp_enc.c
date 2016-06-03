#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/stat.h>

/* This source code modified from:
 * http://www.linuxhowtos.org/data/6/client.c
 */


void error(const char *msg);
int init(int portno);
void check_argc(int argc);
void check_bad_chars(char *file_name);
void check_bad_key(char *plaintext, char *key);
void send_max(int sockfd, char *argv[]);


int main(int argc, char *argv[])
{
	// Usage: otp_enc plaintext key port
	check_argc(argc);
	check_bad_chars( argv[1] );
	check_bad_key( argv[1], argv[2] );
	int portno = atoi(argv[3]);
	int sockfd = init(portno);
	send_max(sockfd, argv);
	close(sockfd);
    return 0;
}

// Add arg for setting error number
//  1 invalid args or
//  2 cannot find port
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int init(int portno) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno); 
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Error: could not contact otp_enc_d on port %d\n", serv_addr.sin_port);
		exit( 2 );
	}
    return sockfd;
}


void check_argc(int argc) {
	if ( argc < 4 ) {
		printf( "Usage: opt_enc plaintext key port\n" );
		exit( 1 );
	}
}

void check_bad_chars(char *file_name) {
	FILE *file = fopen( file_name, "r" );
	int c;
	while ( ( c = fgetc( file ) ) != EOF ) {
		// Check for ascii range 65 to 90 (A through Z), space 32, and linefeed 10
		if ( c != 10 && c != 32 && ( c < 65 || c > 90 ) ) {
			fprintf( stderr, "otp_enc error: input contains bad characters\n" );
			fclose( file );
			exit( 1 );
		}
	}
	fclose( file );
}

int get_char_count( char *file_name ) {
	FILE *file = fopen( file_name, "r" );
	int fd = fileno( file );
	struct stat file_stat;
	fstat( fd, &file_stat );
	int size = file_stat.st_size;
	printf( "size: %d\n", size );
	fclose( file );
	return size;
}

void check_bad_key(char *plaintext, char *key) {
	int plaintext_size = get_char_count( plaintext );
	int key_size = get_char_count( key );
	if ( key_size < plaintext_size ) {
		fprintf( stderr, "Error: key '%s' is too short\n", key );
		exit( 1 );
	}
}

/* Receives an open sockfd and should leave it open for the calling method to close upon return. */
// In the server, it should suffice to have a char array of 100,000 to receive the message.
void send_max(int sockfd, char *argv[]) {
	// receive new port number from server

	// send data to server and receive response from server
	char buffer[100000];
	int n;
    bzero( buffer, 100000 );
	FILE *send_file = fopen( argv[1], "r" );
    fgets( buffer, 99999, send_file );
    n = write( sockfd, buffer, strlen(buffer) );
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer, 100000);
    n = read(sockfd,buffer,99999);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("Buffer from server: %s\n",buffer);
}



