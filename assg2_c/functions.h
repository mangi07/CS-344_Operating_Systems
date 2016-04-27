//#include <unistd.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void setDirectory(char **name);
 
int randInRange(int min, int max);

int makeConnection( int first, int second, char* rooms[], char* directory );
 
/**************************************************************
 * Returns 0 on success and -1 if not enough rooms are provided.
 * ALL_ROOMS_SIZE must be the size of allRooms array.
 * Assumes each name in allRooms array is unique.
 *************************************************************/
int makeRooms(char* allRooms[], int ALL_ROOMS_SIZE, char *directory );

int randInRange(int min, int max); 

int isInArray(char *room, char * rooms[], int size);

void initRoom(int room_index, char* rooms[], int connections, char *directory );

/*************************************************************************************
 * Inspired by https://randomascii.wordpress.com/2013/04/03/stopusingstrncpyalready/ 
 * Returns 1 on error or 0 on success, 1 if destination char array is too small to 
 * contain pSrc char array.
 ************************************************************************************/
int strcat_safe(char destination[], int destCapacity, char *pSrc);
void makeRoomFiles( char* rooms[], char* directory ); 
int connectionExists( int room_one_index, int room_two_index, char* rooms[] ); 
int countConnections( FILE *file);
void addRoomTypes( char* rooms[], char* directory );

#endif

