
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DESTINATION_SIZE 15		//max destination length including the null terminator
#define DATE_SIZE 9				//max date length (DDMMYYYY) including the null terminator
#define BUFFER_SIZE 121			//size of the buffer used to read one line from stdin
#define STOP_INPUT "."			//destination or date that ends the flight input
#define SUCCESS 0
#define FAILURE 1

//Information about one flight. destination and date are dynamically allocated
typedef struct FlightInfo
{
	char* destination;
	char* date;
	float fare;
} FlightInfo;

//One node of the doubly linked list
typedef struct FlightNode
{
	FlightInfo flight;
	struct FlightNode* previousFlight;
	struct FlightNode* nextFlight;
} FlightNode;

//Function prototype

int getString(char* buf, int size);
int getValidString(char* buf, int size);
int getFare(float* fare);
FlightNode* createNewNode(char* destination, char* date, float fare);
void insertNewHead(FlightNode** head, FlightNode** tail, FlightNode* newFlight);
void insertAfter(FlightNode** tail, FlightNode* current, FlightNode* newFlight);
int insertSortedByFare(FlightNode** head, FlightNode** tail, char* destination, char* date, float fare);
int insertSortedByDestination(FlightNode** head, FlightNode** tail, char* destination, char* date, float fare);
int getFlights(FlightNode** fareHead, FlightNode** fareTail, FlightNode** destinationHead, FlightNode** destinationTail);
void printList(FlightNode* head);
FlightNode* findFlight(FlightNode* head, char* destination, char* date);
void deleteNode(FlightNode* node, FlightNode** head, FlightNode** tail);
int changeFare(FlightNode** fareHead, FlightNode** fareTail, FlightNode* destinationHead);
void freeList(FlightNode** head, FlightNode** tail);


int main(void)
{
	//head and tail of the list sorted by fare, at first the list is empty
	FlightNode* fareHead = NULL;
	FlightNode* fareTail = NULL;

	//head and tail of the list sorted by destination, at first the list is empty
	FlightNode* destinationHead = NULL;
	FlightNode* destinationTail = NULL;

	//1. Get all the flights from the user and add them to both lists
	int status = getFlights(&fareHead, &fareTail, &destinationHead, &destinationTail);

	if (status == SUCCESS)
	{
		//2. Print both lists
		printList(fareHead);
		printList(destinationHead);

		//3. Search for one flight and change its fare
		status = changeFare(&fareHead, &fareTail, destinationHead);
	}

	if (status == SUCCESS)
	{
		//4. Print both lists again
		printList(fareHead);
		printList(destinationHead);
	}

	//5. Clear out both lists
	freeList(&fareHead, &fareTail);
	freeList(&destinationHead, &destinationTail);

	return status;
}


