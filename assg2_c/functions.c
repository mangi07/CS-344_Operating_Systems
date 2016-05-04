#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


struct Moves {
	char* room;
	struct Moves* next;
};


void setDirectory(char **name); 
int makeRooms(char* allRooms[], int ALL_ROOMS_SIZE, char* directory, char* chosenRooms[], int CHOSEN_ROOMS_SIZE );
int randInRange(int min, int max); 
void chooseRooms( char* allRooms[], int ALL_ROOMS_SIZE, char* usedRooms[], int USED_ROOMS_SIZE );
int isInArray(char *room, char * rooms[], int size); 
void makeRoomFiles( char* rooms[], char* directory ); 
int strcat_safe(char *destination, int destCapacity, char *pSrc);
void initRoom(int room_index, char* rooms[], int connections, char* directory );
int countConnections( FILE *file);
int makeConnection( int first, int second, char* rooms[], char* directory ); 
int connectionExists( FILE *file_1, FILE *file_2, char *room_1, char *room_2 ); 
void addRoomTypes( char* rooms[], char* directory ); 
void listLocations( char *room, char *directory );
void presentCurrentLocation( char *loc );
void promptUser( char **room, char* directory, struct Moves* moves, int* moves_count );
void recordMove( char* room, struct Moves* moves );
void printEndingMessage( struct Moves* moves, int moves_count );


int main(void)
{

	srand(time(0));
	
	int SIZE = 10;
	char* rooms[] = {"blue", "red", "ONE", "TWO", "Big", "Small", "best", "worst", "CARPET", "WOOD"};
	int CHOSEN_ROOMS_SIZE = 7;
	char * chosenRooms[] = { "", "", "", "", "", "", "" };
	char *directory_name;
	// initialize some variables and set up room files
	setDirectory( &directory_name );
	makeRooms( rooms, SIZE, directory_name, chosenRooms, CHOSEN_ROOMS_SIZE );
	char* curr_loc = (char*)malloc( 100 * sizeof(char) );
	strcpy( curr_loc, chosenRooms[0] );	// the first chosen room is always START_ROOM
										// (the last chosen room is always END_ROOM)
	
	struct Moves* moves = (struct Moves*) malloc( sizeof( struct Moves ) );
	stpcpy( moves->room, curr_loc);
	moves->next = NULL;
	int moves_count = 0;
	int* moves_count_p = &moves_count;
	
	
	int isEnd = 0;
	while ( ! isEnd ) {
		presentCurrentLocation( curr_loc );
		listLocations( curr_loc, directory_name );
		promptUser( &curr_loc, directory_name, moves, moves_count_p );
		// Check if user is in END_ROOM
		if ( ! strcmp( curr_loc, chosenRooms[CHOSEN_ROOMS_SIZE - 1] ) ) {
			printEndingMessage( moves, moves_count );
			// break out of while loop
			isEnd = 1;
		}
	}
	
	
	//clean up
	free( directory_name );
	free( curr_loc );
	int i;
	for ( i = 0; i < CHOSEN_ROOMS_SIZE; ++i ) {
		free( chosenRooms[i] );
	}
	while( moves->next != NULL ) {
		struct Moves* temp = moves->next;
		free( moves );
		moves = temp;
	}
	free( moves );

	exit(0);
}

void printEndingMessage( struct Moves* moves, int moves_count ) {
	printf( "\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n" );
	printf( "YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", moves_count );
	
	if ( moves->next == NULL ) {
		printf( "Internal error: There must be at least two moves!\n" );
		exit( 1 );
	} else {
		moves = moves->next;
	}
	
	do {
		printf( "%s\n", moves->room );
		moves = moves->next;
	} while ( moves->next != NULL );
	printf( "%s\n", moves->room );
}

/***************************************************
 * room is user's current location
 * directory is the room's file directory
 **************************************************/
void promptUser( char **room, char* directory, struct Moves* moves, int* moves_count ) {
	printf( "WHERE TO?>" );
	char to[100] = "";
	fgets( to, 100, stdin );
	
	// replace '\n' with '\0' for string comparison
	char *newline;
	int index;
	newline = strchr(to, '\n');
	index = (int)(newline - to);
	to[index] = '\0';
	
	char path[100];
	sprintf( path, "%s/%s", directory, *room );
	FILE *file = fopen( path, "r" );
	
	char other_1[100] = "";
	char other_1a[100] = "";
	char *name = (char*)malloc( 100 * sizeof(char) );
	while( fscanf( file, "%s %s %s\n", other_1, other_1a, name ) == 3 ) {
		if ( ! strcmp( other_1, "CONNECTION" ) && 
			 ! strcmp( name, to ) ) {
			free( *room );
			*room = name;
			recordMove( *room, moves );
			fclose( file );
			(*moves_count)++;
			return; 
		}
	}
	printf( "\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n" );
	fclose( file );
	
}


void recordMove( char* room, struct Moves* moves ) {
	struct Moves *new_move = (struct Moves*) malloc( sizeof( struct Moves ) );
	new_move->room = (char*)malloc( 100 * sizeof(char) );
	strcpy( new_move->room, room );
	new_move->next = NULL;
	
	while( moves->next != NULL ) {
		moves = moves->next;
	}
	// Now we can add a new move to the end.
	moves->next = new_move;
}


void presentCurrentLocation( char *loc ) {
	printf( "\nCURRENT LOCATION: %s\n", loc );
}

void listLocations( char *room, char *directory ) {
	char other_1[100] = "";
	char other_1a[100] = "";
	char name[100] = "";
	
	char path[100];
	sprintf( path, "%s/%s", directory, room );
	FILE *file = fopen( path, "a+" );
	
	int connections = countConnections( file ) - 1; // countConnections actually counts the number of lines in file - 1 
	fseek( file, 0, SEEK_SET );
	
	
	int count = 0;
	printf( "POSSIBLE CONNECTIONS: " );
	while( fscanf( file, "%s %s %s\n", other_1, other_1a, name ) == 3 ) {
		if ( ! strcmp( other_1, "CONNECTION" ) ) {
			printf( "%s", name );
			if (count + 1 < connections)
				printf( ", " );
			count++;
		}
	}
	printf( ".\n" );
	
	fclose( file );
}



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


/**************************************************************
 * Returns 0 on success and -1 if not enough rooms are provided.
 * ALL_ROOMS_SIZE must be the size of allRooms array.
 * Assumes each name in allRooms array is unique.
 *************************************************************/
int makeRooms(char* allRooms[], int ALL_ROOMS_SIZE, char* directory, 
			  char* chosenRooms[], int CHOSEN_ROOMS_SIZE ) {
	//int USED_ROOMS_SIZE = 7;
	if (ALL_ROOMS_SIZE < CHOSEN_ROOMS_SIZE) {
		return -1;
	}
	
	chooseRooms( allRooms, ALL_ROOMS_SIZE, chosenRooms, CHOSEN_ROOMS_SIZE );
	// chosenRooms array is now filled with unique room names
	
	// add the first line to all the files
	makeRoomFiles( chosenRooms, directory ); 
	
	// add connections to all the files
	int connections;
	int MIN_CONNECTIONS = 3;
	int MAX_CONNECTIONS = 6;
	int i;
	for ( i = 0; i < CHOSEN_ROOMS_SIZE; ++i ) {
		connections = randInRange(MIN_CONNECTIONS, MAX_CONNECTIONS);
		initRoom(i, chosenRooms, connections, directory);
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


/**********************************************************************
 * Assigns a subset of allRooms to usedRooms.
 * Rooms are not assigned if allRooms size is less than usedRooms size.
 *********************************************************************/
void chooseRooms( char* allRooms[], int ALL_ROOMS_SIZE, char* usedRooms[], int USED_ROOMS_SIZE ) {
	if ( ALL_ROOMS_SIZE < USED_ROOMS_SIZE ) {
		printf( "in chooseRooms: ALL_ROOMS_SIZE must be at least 7\n" );
		exit(1);
	}

	char *random_room = allRooms[randInRange(0, USED_ROOMS_SIZE - 1)];
	int count = 0;
	char *chosen_room = NULL;
	for ( count; count < USED_ROOMS_SIZE; ++count ) {
		while (isInArray(random_room, usedRooms, USED_ROOMS_SIZE - 1)) {
			 random_room = allRooms[randInRange(0, ALL_ROOMS_SIZE - 1)];
		}
		chosen_room = (char*)malloc( 100 * sizeof(char) );
		strcpy( chosen_room, random_room );
		usedRooms[count] = chosen_room;
	}

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
	char room_dir[100] = "";
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
		int rand_index = randInRange( 0, 6 );
		if ( makeConnection( room_index, rand_index, rooms, directory ) ) {
			connections_to_make--;
		}
	}

	fclose(file_p);
}


/**************************************************************
 * Takes an open file and finds its number of connections
 *************************************************************/
int countConnections( FILE *file){
	// We compensate for the first line of the file that indicates room name
	int connections = -1;
	fseek( file, 0, SEEK_SET );
	char buffer[100];
	// We count the number of lines in the file.
	while ( fgets( buffer, sizeof buffer, file ) != NULL ) {
		connections++;
	}
	// At this point, connections should be 1 less than the number of lines in the file,
	//printf( "in countConnections: connections = %d\n", connections );
	
	// return file pointer to the end of the file
	fseek( file, 0, SEEK_END );


	return connections;
}


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


int connectionExists( FILE *file_1, FILE *file_2, char *room_1, char *room_2 ) {
	// move both file pointers to the beginning
	fseek( file_1, 0, SEEK_SET );
	fseek( file_2, 0, SEEK_SET );

	int first_check = 0;
	int second_check = 0;

	// check first file for second room name
	char other_1[100] = "";
	char other_1a[100] = "";
	char name_1[100] = "";
	// or check ( ! feof( file_1 ) )
	while( fscanf( file_1, "%s %s %s\n", other_1, other_1a, name_1 ) == 3 ) {
		if ( ! strcmp( name_1, room_2 ) ) {
			first_check = 1;
			break;
		}
	}

	// check second file for first room name
	char other_2[100] = "";
	char other_2a[100] = "";
	char name_2[100] = "";
	while( fscanf( file_2, "%s %s %s\n", other_2, other_2a, name_2 ) == 3 ) {
		if ( ! strcmp( name_2, room_1 ) ) {
			second_check = 1;
			break;
		}
	}

	// return the file pointers to end position
	fseek( file_1, 0, SEEK_END );
	fseek( file_2, 0, SEEK_END );

	// a connection exists only if both files indicate the connection
	return ( first_check && second_check );
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




