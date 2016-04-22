#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main(void) {

/*****************************************************************************
 *  Test isInArray
 ****************************************************************************/
	int arr[] = {1, 2, 3};
	int arrSize = 3;

	int returnStatus = isInArray(5, arr, arrSize);
	printf("In tester...isInArray expected: 0, actual: %d \n", returnStatus);

	returnStatus = isInArray(3, arr, arrSize);
	printf("In tester...isInArray expected: 1, actual: %d \n", returnStatus);
	printf("End Test isInArray\n\n");
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
 *  Test initRoom
 ****************************************************************************/
	

/*****************************************************************************
 *  Test strcat_safe
 ****************************************************************************/
	int SIZE = 3;
	char buffer[SIZE];
	char addedText[] = "Thisisalongstrong";
	returnStatus = strcat_safe(buffer, SIZE, addedText);
	printf("In tester...strcat_safe expected: 1, actual: %d \n", returnStatus);
	printf("In tester...strcat_safe buffer: %s \n", buffer);


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


	printf("In tester...end of tests.\n");

	exit(0);
}
