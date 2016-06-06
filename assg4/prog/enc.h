/* File: enc.h
 * Author: Ben R. Olson
 * Date: June 5, 2016
 */

#ifndef ENC_H
#define ENC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CONVERT
#define CONVERT
// scale buffer and key chars for upcoming integer arithmetic
int convert( char c ) {
	if ( c >= 'A' && c <= 'Z' ) c = c - 'A';
	else if ( c == ' ' ) c = 26;
	//printf( "In convert, c = %d\n", c );
	return c;
}


// check that buffer and key contain only A - Z and space with newline at the end
void clean_buffer( char* buffer ) {
	int i = 0;
	char temp_buffer[100000];
	while ( buffer[i] && ( buffer[i] < 'A' || buffer[i] > 'Z' ) && buffer[i] != ' ' ) {
		i++;
		printf( "in clean_buffer while loop, buffer[%d] = %c\n", i, buffer[i] );
	}
	// copy the clean portion to temporary buffer, clear original buffer, and then copy back
	strncpy( temp_buffer, buffer + i, 99999 );
	bzero( buffer, 100000 );
	strncpy( buffer, temp_buffer, 99999 );
	printf( "in clean_buffer at end, buffer holds %s\n", buffer );
}
#endif

/* Expected: buffer passed in must be no longer than 99999 characters
 * and end with newline character,
 * and key passed in must be at least as long as the buffer
 * 
 * Return: buffer encrypted in place */
void enc( char* buffer, char* key ) {



	clean_buffer( buffer);
	// loop over all the characters until you reach newline character
	// encrypting each character in place as you go
	int i = 0;
	while ( buffer[i] ) {
		//printf( "buffer[%d]: %d\n", i, buffer[i] );

		// skip over (don't encrypt) invalid characters
		if ( buffer[i] && ( buffer[i] < 'A' || buffer[i] > 'Z' ) && buffer[i] != ' ' ) {
			i++;
			continue;		
		}
		//sleep( 1 );

		int b = convert( buffer[i] );
		int k = convert( key[i] );

		// next character at buffer[i] is encrypted here
		b = b + k;
		if ( b > 26 )
			b = b - 27;
		// replace original character at i with encryption,
		// modifying the original buffer
		// Note: anything that is encrypted as space ' ' is b = 26
		//   so in those cases, buffer[i] will be assigned the space character
		if ( b == 26 )
			buffer[ i ] = ' ';
		else
			buffer[ i ] = b + 'A';

		i++;
		// loop safeguard
		if ( i > 99999 ) break;
	}
}





#endif
