#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "functions.h"

//int randInRange(int min, int max);
//int isInArray(int arrElement, int arr[], int size);
//void initRoom(int index, int connections);


/***************************************
 * Call this function before any others.
 * Caller must remember to free(name).
 **************************************/
void setDirectory(char *name) {
	// getpid() returns type pit_t which I'm guessing could
	// be type unsigned int, so 100 chars should be enough
	// to hold olsonbe.rooms.<pid> where the width of 
	// the character representation of the pid will probably 
	// be NO longer than the character width representation 
	// of the max value of unsigned int.
	name = malloc( sizeof(char) * 100 );
	sprintf( name, "%s%ld", "olsonbe.rooms.", getpid() );
	mkdir( name, ACCESSPERMS );
	printf( "%s\n", name );
}
	


/**********************************************************************
 * Assigns a subset of allRooms to usedRooms.
 * Rooms are not assigned if allRooms size is less than usedRooms size.
 *********************************************************************/
void chooseRooms( char* allRooms[], int ALL_ROOMS_SIZE, char* usedRooms[], int USED_ROOMS_SIZE ) {
	if ( ALL_ROOMS_SIZE < USED_ROOMS_SIZE ) {
		printf( "in chooseRooms: ALL_ROOMS_SIZE must be at least 7\n" );
		exit(1);
	}

	char *chosen_room = allRooms[randInRange(0, ALL_ROOMS_SIZE - 1)];
	int count = 0;
	for ( count; count < USED_ROOMS_SIZE; ++count ) {
		while (isInArray(chosen_room, usedRooms, USED_ROOMS_SIZE - 1)) {
			 chosen_room = allRooms[randInRange(0, ALL_ROOMS_SIZE)];
		}
		usedRooms[count] = chosen_room;
		printf( "usedRooms[%d]: %s\n", count, chosen_room );
	}
}

/**************************************************************
 * Returns 0 on success and -1 if not enough rooms are provided.
 * ALL_ROOMS_SIZE must be the size of allRooms array.
 * Assumes each name in allRooms array is unique.
 *************************************************************/
int makeRooms(char* allRooms[], int ALL_ROOMS_SIZE) {
	int USED_ROOMS_SIZE = 7;
	if (ALL_ROOMS_SIZE < USED_ROOMS_SIZE) {
		return -1;
	}
	
	char * usedRooms[] = {"", "", "", "", "", "", ""};
	chooseRooms( allRooms, ALL_ROOMS_SIZE, usedRooms, USED_ROOMS_SIZE );
	// usedRooms array is now filled with unique room names
	
	int connections;
	int MIN_CONNECTIONS = 3;
	int MAX_CONNECTIONS = 6;

	for ( int i = 0; i < USED_ROOMS_SIZE; ++i ) {
		int connections = randInRange(MIN_CONNECTIONS, MAX_CONNECTIONS);
		initRoom(i, usedRooms, connections); // define this later
	}

	return 0;	
}



/******************************************************************************
 * Preconditions: Please seed with srand(time(0)) before calling this function.
 *****************************************************************************/
int randInRange(int min, int max) {
	int num = ( rand() % (max - (min - 1)) ) + min;
	return num;
}


int isInArray(char *room, char * rooms[], int size) {
	int i;
	for (i = 0; i < size; ++i) {
		if ( ! strcmp( room, rooms[i] ) ) {
			return 1;
		}
	}
	return 0;
}


// finish
int countConnections(char room_name[]){
	return 0;
}

// finish
int connectionExists( int room_one_index, int room_two_index, char* rooms[] ) {
	return 0;
}

// finish
/******************************************************************************
 * Checks to see whether the two rooms identified by index are the same,
 * and returns 0 if they are (avoiding a room making a connection to itself).
 *
 * If the two rooms identified are different, a connection will be made
 * between them unless the connection already exists.
 *
 * If the second room file (identified by rand_index) does not yet exist,
 * that second room will be created with the connection back to the other room.
 *****************************************************************************/
int makeConnection( int first, int second, char* rooms[] ) {
	if ( first == second )
		return 0;

	// get room names
	char *first_room_name = rooms[first];
	char *second_room_name = rooms[second];
	
	// Open room files for reading and writing
	FILE *first_room_file = fopen( first_room_name, "a+" );
	FILE *second_room_file = fopen( second_room_name, "a+" );
	
	// Close both files
	fclose( first_room_file );
	fclose( second_room_file );

	printf( "in makeConnection: first\t%d \tsecond\t%d\n", first, second );
	return 1;	
}


/************************************************************************
 * int room_index	Identifies the room to initialize.
 * char* rooms[]	Array of room names, each to correspond to a file.
 * int connections	Total number of connections the room 
 * 			identified by room_index should have.
 ***********************************************************************/
void initRoom(int room_index, char* rooms[], int connections) {
	char *room_name = rooms[room_index];
	FILE *file_p = fopen( room_name, "a+" );

	int connections_in_file = countConnections( room_name );
	int connections_to_make = connections - connections_in_file;

	while ( connections_to_make > 0 ) {
		int rand_index = randInRange( 0, 7 );
		if ( makeConnection( room_index, rand_index, rooms ) ) {
			connections_to_make--;
		}
	}

	fclose(file_p);
}



/*************************************************************************************
 * Inspired by https://randomascii.wordpress.com/2013/04/03/stopusingstrncpyalready/ 
 * Returns 1 on error or 0 on success, 1 if destination char array is too small to 
 * contain pSrc char array.
 ************************************************************************************/
int strcat_safe(char destination[], int destCapacity, const char *pSrc) {
	int capacityNeeded = strlen(destination) + strlen(pSrc) + 1; // add one for null character
	printf("in strcat_safe...strlen(destination) = %d \n", strlen(destination));
	printf("in strcat_safe...capacityNeeded = %d \n", capacityNeeded);

	if (destCapacity < capacityNeeded) {
		return 1;
	}

	strcat(destination, pSrc);
	// ensure null termination
	destination[strlen(destination)] = 0;
	
	return 0;
}

/*
int xmain(void)
{
	// test isInArray
	int arr[] = {1, 2, 3};
	int arrSize = 3;
	int returnStatus = isInArray(5, arr, arrSize);
	// display an assertion here
	printf("expected: 0, actual: %d", returnStatus);

	//char* rooms[] = {"blue", "red", "ONE", "TWO", "Big", "Small", "best", "worst", "CARPET", "WOOD"};
	//int SIZE = 10;

	//makeRooms(rooms, SIZE);
	exit(0);
}
*/


