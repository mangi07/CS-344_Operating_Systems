#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "functions.h"

int main(void) {


// Setup
int returnStatus;

/*****************************************************************************
 *  Test isInArray
 ****************************************************************************/
	//int arr[] = {1, 2, 3};
	//int arrSize = 3;

	//int returnStatus = isInArray(5, arr, arrSize);
	//printf("In tester...isInArray expected: 0, actual: %d \n", returnStatus);

	//returnStatus = isInArray(3, arr, arrSize);
	//printf("In tester...isInArray expected: 1, actual: %d \n", returnStatus);
	//printf("End Test isInArray\n\n");
/*****************************************************************************
 *  Test randInRange 
 ****************************************************************************/
	//int MIN = 0;
	//int MAX = 5;
	//int randomNumber = randInRange(MIN, MAX);
	//srand( time(0) );
	//for (int i = 0; i < 100; ++i) {
	//	printf("In tester...randInRange expected in range (%d, %d), actual: %d \n", MIN, MAX, randomNumber);
	//	randomNumber = randInRange(0, 5);
	//}
	//printf("End Test randInRange\n\n");


/*****************************************************************************
 *  Test strcat_safe
 ****************************************************************************/
/*
	// capacity too small
	const int SIZE = 3;
	char firstBuffer[3] = "";
	printf("strlen(firstBuffer) = %d \n", strlen(firstBuffer));
	char addedText[] = "Thisisalongstring"; // 17 characters + null terminator
	returnStatus = strcat_safe(firstBuffer, SIZE, addedText);
	printf("Capacity really needed: 18\n");
	printf("In tester...strcat_safe expected: 1, actual: %d \n", returnStatus);
	printf("In tester...strcat_safe buffer: %s \n", firstBuffer);
	printf("Number of characters in buffer: %d \n\n", strlen(firstBuffer));

	// capacity plenty big
	const int NEW_SIZE = 100;
	char newBuffer[100] = "";
	returnStatus = strcat_safe(newBuffer, NEW_SIZE, addedText);
	printf("Capacity really needed: 18\n");
	printf("In tester...strcat_safe expected: 0, actual: %d \n", returnStatus);
	printf("In tester...strcat_safe newBuffer expected 'Thisisalongstring', actual: %s \n", newBuffer);
	printf("Number of characters in buffer: %d \n\n", strlen(newBuffer));

	returnStatus = strcat_safe(newBuffer, NEW_SIZE, addedText);
	printf("Capacity really needed: 35\n");
	printf("In tester...strcat_safe expected: 0, actual: %d \n", returnStatus);
	printf("In tester...strcat_safe newBuffer expected 'ThisisalongstringThisisalongstring', actual: %s \n", newBuffer);
	printf("Number of characters in buffer: %d \n\n", strlen(newBuffer));

	// edge case: capacity barely enough
	const int LAST_SIZE = 10;
	char lastBuffer[10] = "abcd";
	char lastAdded[] = "efghi";
	returnStatus = strcat_safe(lastBuffer, LAST_SIZE, lastAdded);
	printf("Capacity really needed: 10\n");
	printf("In tester...strcat_safe expected: 0, actual: %d \n", returnStatus);
	printf("In tester...strcat_safe lastBuffer expected 'abcdefghi', actual: %s \n", lastBuffer);
	printf("Number of characters in buffer: %d \n\n", strlen(lastBuffer));

	// edge case: capacity one char too short
	returnStatus = strcat_safe(lastBuffer, LAST_SIZE, "j");
	printf("Capacity really needed: 11\n");
	printf("In tester...strcat_safe expected: 1, actual: %d \n", returnStatus);
	printf("In tester...strcat_safe lastBuffer expected 'abcdefghi', actual: %s \n", lastBuffer);
	printf("Number of characters in buffer: %d \n\n", strlen(lastBuffer));
*/

/*****************************************************************************
 *  Test makeRooms
 ****************************************************************************/
	//char* rooms[] = {"blue", "red", "ONE", "TWO", "Big", "Small", "best", "worst", "CARPET", "WOOD"};
	//int SIZE = 10;

	//srand( time(0) );
	//returnStatus = makeRooms(rooms, SIZE);
	//printf("In tester...makeRooms expected: 0, actual: %d \n", returnStatus);
	//printf("End Test makeRooms\n\n");
	//

/*****************************************************************************
 *  Test initRoom
 ****************************************************************************/
/*
	char* rooms_2[] = {"blue", "red", "ONE", "TWO", "Big", "Small", "best", "worst", "CARPET", "WOOD"};
	int SIZE_2 = 10;


	srand( time( 0 ) );
	initRoom(0, rooms_2, 6);
*/


/******************************************************************************
 * Test setDirectory
 *****************************************************************************/
	char *directory_name;
	setDirectory(directory_name);
	free(directory_name);


	printf("In tester...end of tests.\n");

	exit(0);
}
