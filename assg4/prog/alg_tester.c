/* File: alg_tester.c
 * Author: Ben R. Olson
 * Date: June 5, 2016
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enc.h"
#include "dec.h"

int main(int argc, char *argv[])
{
	// expects two arguments: buffer and then key
	if ( argc < 3 ) {
		printf( "Usage: alg_tester buffer key\n\n" );
		exit( 1 );
	}
	
	// encryption
	printf( "Original buffer: %s\n\n", argv[ 1 ] );
	printf( "Original key: %s\n\n", argv[ 2 ] );

	enc( argv[ 1 ], argv[ 2 ] );

	
	printf( "Encrypted buffer: %s\n\n", argv[ 1 ] );

	// decryption
	dec( argv[ 1 ], argv[ 2 ] );

	printf( "Decrypted buffer: %s\n\n", argv[ 1 ] );

	return 0;
}


