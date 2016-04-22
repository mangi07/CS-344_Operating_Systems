//#include <unistd.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int randInRange(int min, int max);

int isInArray(int arrElement, int arr[], int size);

/**************************************************************
 * Returns 0 on success and -1 if not enough rooms are provided.
 * ALL_ROOMS_SIZE must be the size of allRooms array.
 * Assumes each name in allRooms array is unique.
 *************************************************************/
int makeRooms(char* allRooms[], int ALL_ROOMS_SIZE);

int randInRange(int min, int max); 

int isInArray(int arrElement, int arr[], int size);

void initRoom(char roomName[], int connections);

/*************************************************************************************
 * Inspired by https://randomascii.wordpress.com/2013/04/03/stopusingstrncpyalready/ 
 * Returns 1 on error or 0 on success, 1 if destination char array is too small to 
 * contain pSrc char array.
 ************************************************************************************/
int strcat_safe(char destination[], int destCapacity, const char *pSrc);

#endif

