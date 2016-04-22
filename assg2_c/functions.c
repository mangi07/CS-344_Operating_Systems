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
	int usedRooms[] = {-1, -1, -1, -1, -1, -1, -1};
	int count = 0;
	int connections;
	int MIN_CONNECTIONS = 3;
	int MAX_CONNECTIONS = 6;

	// keep track of allRooms indices in usedRooms array
	// so that each initialized room will have a unique name
	while (count < USED_ROOMS_SIZE) {
		int allRoomsIndex = randInRange(0, ALL_ROOMS_SIZE - 1);
		while (isInArray(allRoomsIndex, usedRooms, USED_ROOMS_SIZE - 1)) {
			allRoomsIndex = randInRange(0, ALL_ROOMS_SIZE);
		}
		int connections = randInRange(MIN_CONNECTIONS, MAX_CONNECTIONS);
		initRoom(allRooms[allRoomsIndex], connections); // define this later
		usedRooms[count] = allRoomsIndex;
		count++;
		
		// remove when done with testing
		printf("In functions.c...makeRooms...allRoomsIndex = %d \n", allRoomsIndex);

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


int isInArray(int arrElement, int arr[], int size) {
	int searchIndex;
	for (searchIndex = 0; searchIndex < size; ++searchIndex) {
		if (arrElement == arr[searchIndex]) {
			return 1;
		}
	}
	return 0;
}


//test
void initRoom(char roomName[], int connections) {
	int fileDescriptor;
	char buffer[100];
	// finish
}


//test
/*************************************************************************************
 * Inspired by https://randomascii.wordpress.com/2013/04/03/stopusingstrncpyalready/ 
 * Returns 1 on error or 0 on success, 1 if destination char array is too small to 
 * contain pSrc char array.
 ************************************************************************************/
int strcat_safe(char destination[], int destCapacity, const char *pSrc) {
	int capacityNeeded = destCapacity + strlen(pSrc);
	printf("in strcat_safe...capacityNeeded = %d \n", capacityNeeded);

	if (destCapacity < capacityNeeded) {
		return 1;
	}
	strcat(destination, pSrc);
	// ensure null termination
	destination[strlen(destination) - 1] = 0;
	return 0;
}


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
