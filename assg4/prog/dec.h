/* File: dec.h
 * Author: Ben R. Olson
 * Date: June 5, 2016
 */

#ifndef DEC_H
#define DEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CONVERT
#define CONVERT
// scale buffer and key chars for upcoming integer arithmetic
int convert( char c ) {
	if ( c >= 'A' && c <= 'Z' ) c = c - 'A';
	else if ( c == ' ' ) c = 26;
	return c;
}

// check that buffer and key contain only A - Z and space with newline at the end
// buffer must be size 100000
void clean_buffer( char* buffer ) {
	int i = 0;
	char temp_buffer[100000];
	while ( buffer[i] && ( buffer[i] < 'A' || buffer[i] > 'Z' ) && buffer[i] != ' '  ) {
		i++;
	}
	// copy the clean portion to temporary buffer, clear original buffer, and then copy back
	strncpy( temp_buffer, buffer + i, 99999 );
	bzero( buffer, 100000 );
	strncpy( buffer, temp_buffer, 99999 );

}
#endif

/* Expected: buffer passed in must be size 100000
 * and end with newline character,
 * and key passed in must be at least as long as the buffer
 * 
 * Return: buffer decrypted in place */
void dec( char* buffer, char* key ) {
	clean_buffer( buffer );
	// loop over all the characters until you reach newline character
	// decrypting each character in place as you go
	int i = 0;
	while ( buffer[i] ) {
		// skip over (don't decrypt) invalid characters
		if ( buffer[i] && ( buffer[i] < 'A' || buffer[i] > 'Z' ) && buffer[i] != ' ' ) {
			i++;
			continue;		
		}

		int b = convert( buffer[i] );
		int k = convert( key[i] );

		// next character at buffer[i] is decrypted here
		b = b - k;
		if ( b < 0 )
			b = b + 27;
		// replace original character at i with decryption,
		// modifying the original buffer
		// Note: anything that is decrypted as space ' ' is b = 26
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

