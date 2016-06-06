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
#endif

/* Expected: buffer passed in must be no longer than 99999 characters
 * and end with newline character,
 * and key passed in must be at least as long as the buffer
 * 
 * Return: buffer encrypted in place */
void enc( char* buffer, char* key ) {

	// loop over all the characters until you reach newline character
	// encrypting each character in place as you go
	int i = 0;
	while ( buffer[i] ) {
		printf( "buffer[%d]: %d\n", i, buffer[i] );
		sleep( 1 );

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
		if ( buffer[ i ] == 26 )
			buffer[ i ] = ' ';
		else
			buffer[ i ] = b + 'A';

		i++;
		// loop safeguard
		if ( i > 99999 ) break;
	}
}

// add method to check that buffer and key contain only A - Z and space with newline at the end




#endif
