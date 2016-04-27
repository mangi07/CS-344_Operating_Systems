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
void setDirectory(char **name) {
	// getpid() returns type pit_t which I'm guessing could
	// be type unsigned int, so 100 chars should be enough
	// to hold olsonbe.rooms.<pid> where the width of 
	// the character representation of the pid will probably 
	// be NO longer than the character width representation 
	// of the max value of unsigned int.
	*name = ( char* ) malloc( sizeof(char) * 100 );
	sprintf( *name, "%s%ld", "olsonbe.rooms.", getpid() );
	mkdir( *name, ACCESSPERMS );
	//printf( "%s\n", *name );
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
		//printf( "usedRooms[%d]: %s\n", count, chosen_room );
	}
}

/**************************************************************
 * Returns 0 on success and -1 if not enough rooms are provided.
 * ALL_ROOMS_SIZE must be the size of allRooms array.
 * Assumes each name in allRooms array is unique.
 *************************************************************/
int makeRooms(char* allRooms[], int ALL_ROOMS_SIZE, char* directory) {
	int USED_ROOMS_SIZE = 7;
	if (ALL_ROOMS_SIZE < USED_ROOMS_SIZE) {
		return -1;
	}
	
	char * chosenRooms[] = {"", "", "", "", "", "", ""};
	chooseRooms( allRooms, ALL_ROOMS_SIZE, usedRooms, USED_ROOMS_SIZE );
	// usedRooms array is now filled with unique room names
	
	// add the first line to all the files
	makeRoomFiles( chosenRooms, directory ); 
	
	// add connections to all the files
	int connections;
	int MIN_CONNECTIONS = 3;
	int MAX_CONNECTIONS = 6;
	int i;
	for ( i = 0; i < USED_ROOMS_SIZE; ++i ) {
		connections = randInRange(MIN_CONNECTIONS, MAX_CONNECTIONS);
		initRoom(i, usedRooms, connections, directory);
	}

	// add the last line to all the files
	addRoomTypes( chosenRooms, directory ); 
	

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


/**************************************************************
 * Takes an open file and finds its number of connections
 *************************************************************/
int countConnections( FILE *file){
	// We compensate for the first line of the file that indicates room name
	int connections = -1;
	rewind( file );
	char buffer[100];
	// We count the number of lines in the file.
	while ( fgets( buffer, sizeof buffer, file ) != NULL ) {
		connections++;
	}
	// At this point, connections should be 1 less than the number of lines in the file,
	//printf( "in countConnections: connections = %d\n", connections );
	
	// return file pointer to the end of the file
	lseek( file, 0, SEEK_END );


	return connections;
}


// finish
int connectionExists( FILE *file_1, FILE *file_2, char *room_1, char *room_2 ) {
	// move both file pointers to the beginning
	rewind( file_1 );
	rewind( file_2 );

	int first_check = 0;
	int second_check = 0;

	// check first file for second room name
	char other_1[100] = "";
	char name_1[100] = "";
	while( fgets( file_1, "%s: %s\n", other_1, name_1 ) != NULL ) {
		if ( ! cmpstr( name_1, room_2 ) ) {
			first_check = 1;
			break;
		}
	}

	// check second file for first room name
	char other_2[100] = "";
	char name_2[100] = "";
	while( fgets( file_2, "%s: %s\n", other_2, name_2 ) != NULL ) {
		if ( ! cmpstr( name_2, room_1 ) ) {
			second_check = 1;
			break;
		}
	}

	// return the file pointers to end position
	lseek( file_1, 0, SEEK_END );
	lseek( file_2, 0, SEEK_END );

	// a connection exists only if both files indicate the connection
	return ( first_check && second_check );
}


// finish
/******************************************************************************
 * Checks to see whether the two rooms identified by index are the same,
 * and returns 0 if they are (avoiding a room making a connection to itself).
 *
 * If the two rooms identified are different, a connection will be made
 * between them unless the connection already exists.
 *****************************************************************************/
int makeConnection( int first, int second, char* rooms[], char* directory ) {
	if ( first == second )
		return 0;

	// printf( "in makeConnection: directory: %s\n", directory );

	// get room names
	char *first_room = rooms[first];
	char *second_room = rooms[second];
	
	// reserve room for file path strings
	char first_dir[100] = "";
	char second_dir[100] = "";


	// make directory/file strings
	strcat_safe( first_dir, 100, directory );
	strcat_safe( first_dir, 100, "/" );
	strcat_safe( first_dir, 100, first_room );

	strcat_safe( second_dir, 100, directory );
	strcat_safe( second_dir, 100, "/" );
	strcat_safe( second_dir, 100, second_room );

	
	// Open room files for reading and writing
	FILE *first_room_file = fopen( first_dir, "a+" );
	FILE *second_room_file = fopen( second_dir, "a+" );

	
	if ( ! ( first_room_file && second_room_file ) ) {
		printf( "in makeConnection: first_dir: %s\n", first_dir );
		printf( "in makeConnection: second_dir: %s\n", second_dir );
		printf( "Error opening file(s) in function makeConnection\n" );
		exit( 1 );
	}


	if ( connectionExists( first_room_file, second_room_file, first_room, second_room ) ) {
		return 1;
	}	

	int c1 = countConnections( first_room_file ) + 1;
	int c2 = countConnections( second_room_file ) + 1;
	fprintf( first_room_file, "CONNECTION %d: %s\n", c1, second_room );
	fprintf( second_room_file,"CONNECTION %d: %s\n", c2, first_room );
	
	// Close both files
	fclose( first_room_file );
	fclose( second_room_file );

	//printf( "in makeConnection: first\t%d \tsecond\t%d\n", first, second );
	//printf( "in makeConnection: dir1:\t%s \tdir2:\t%s\n\n", first_dir, second_dir );
	return 1;	
}


/************************************************************************
 * int room_index	Identifies the room to add connection(s) to.
 * char* rooms[]	Array of room names, each to correspond to a file.
 * int connections	Total number of connections the room 
 * 			identified by room_index should have.
 ***********************************************************************/
void initRoom(int room_index, char* rooms[], int connections, char* directory ) {
	char *room_name = rooms[room_index];
	FILE *file_p = NULL;
	// need to open in directory, so craft the string here
	char room_dir[100];
	strcat_safe( room_dir, 100, directory );
	strcat_safe( room_dir, 100, "/" );
	strcat_safe( room_dir, 100, rooms[room_index] );

	file_p = fopen( room_dir, "a+" );
	// Check for successful file opening.
	if ( ! file_p ) {
		printf( "Error opening file in function initRoom!\n" );
		exit( 1 );
	}

	int connections_in_file = countConnections( file_p );
	int connections_to_make = connections - connections_in_file;

	while ( connections_to_make > 0 ) {
		int rand_index = randInRange( 0, 7 );
		if ( makeConnection( room_index, rand_index, rooms, directory ) ) {
			connections_to_make--;
		}
	}

	fclose(file_p);
}


/******************************************************
 * Call this function at the beginning to create all
 * room files and add the first line to each file.
 *****************************************************/
void makeRoomFiles( char* rooms[], char* directory ) {


	char room_dir[100] = "";
	FILE *file_p = NULL;
	
	int i;
	for ( i = 0; i < 7; ++i ) {
		// prepare directory string
		strcat_safe( room_dir, 100, directory );
		strcat_safe( room_dir, 100, "/" );
		strcat_safe( room_dir, 100, rooms[i] );

		// do initial writes to file
		file_p = fopen( room_dir, "a+" );
		fprintf( file_p, "ROOM NAME: %s\n", rooms[i] );
		fclose( file_p );

		memset( &room_dir[0], 0, sizeof(room_dir) );
	}
}


/*************************************************************
 * Call this function at the very end, but we can still use
 * the files because we know room type by rooms array position
 ************************************************************/
void addRoomTypes( char* rooms[], char* directory ) {
	char room_dir[100] = "";
	FILE *file_p = NULL;
	
	int i;
	for ( i = 0; i < 7; ++i ) {
		// prepare directory string
		strcat_safe( room_dir, 100, directory );
		strcat_safe( room_dir, 100, "/" );
		strcat_safe( room_dir, 100, rooms[i] );

		// do final writes to file
		file_p = fopen( room_dir, "a+" );
		if (i == 0) {
			fprintf( file_p, "ROOM TYPE: START_ROOM\n" );
		} else if (i < 6) {
			fprintf( file_p, "ROOM TYPE: MID_ROOM\n" );
		} else {
			fprintf( file_p, "ROOM TYPE: END_ROOM\n" );
		}
		fclose( file_p );

		memset( &room_dir[0], 0, sizeof(room_dir) );
	}

}


/*************************************************************************************
 * Inspired by https://randomascii.wordpress.com/2013/04/03/stopusingstrncpyalready/ 
 * Returns 1 on error or 0 on success, 1 if destination char array is too small to 
 * contain pSrc char array.
 ************************************************************************************/
int strcat_safe(char *destination, int destCapacity, char *pSrc) {

	if ( ! ( destination && pSrc ) ) {
		printf( "in strcat_safe: null string(s) passed in!\n" );
		exit( 1 );
	}
	//printf("%d\n", strlen( pSrc ) );

	int capacityNeeded = strlen(destination) + strlen(pSrc) + 1; // add one for null character


	//printf("in strcat_safe...strlen(destination) = %d \n", strlen(destination));
	//printf("in strcat_safe...capacityNeeded = %d \n", capacityNeeded);

	if (destCapacity < capacityNeeded) {
		return 1;
	}

	strcat(destination, pSrc);
	// ensure null termination
	destination[strlen(destination)] = 0;
	
	return 0;
}


int main(void)
{
	char* rooms[] = {"blue", "red", "ONE", "TWO", "Big", "Small", "best", "worst", "CARPET", "WOOD"};
	int SIZE = 10;
	
	char *directory_name;
	setDirectory( &directory_name );

	makeRooms( rooms, SIZE, directory_name );

	exit(0);
}



