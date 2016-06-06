/* File: keygen.c
 * Author: Ben R. Olson
 * Date: June 6, 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// return random integer within range, inclusive
int rand_range( int min, int max ) {
	int num = ( rand() % (max - (min - 1)) ) + min;
	return num;
}

// convert number to char
char convert( int number ) {
	char c;
	if ( number == 26 ) c = ' ';
	else c = number + 'A';

	return c;
}
 
int main(int argc, char *argv[]) {
	srand(time(0));

	if ( argc != 2 ) {
		printf( "Usage: keygen <length of key to generate>\nExample:\nkeygen 70000\n\n" );
		exit( 1 );
	}

	int key_length = atoi( argv[1] );
	int i;
	for ( i = 0; i < key_length; ++i ) {
		int n = rand_range( 0, 26 );
		char c = convert( n );
		printf( "%c", c );
	}
	printf( "\n" );

	return 0;
}
