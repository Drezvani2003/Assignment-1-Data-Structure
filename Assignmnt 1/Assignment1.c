
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
//
// FUNCTION    : getString
// DESCRIPTION :
//   This function reads one line from stdin and removes the newline. If the
//   user types more than what fits in buf, the extra characters are dropped
//   so that buf can never overflow.
// PARAMETERS  :
//   char* buf : where the string is stored
//   int size  : size of buf including the null terminator
// RETURNS     :
//   int : SUCCESS if a line was read, FAILURE if there is no more input
//
int getString(char* buf, int size) {
    char temp[BUFFER_SIZE] = "";

    buf[0] = '\0';

    if (fgets(temp, sizeof(temp), stdin) == NULL) {
        return FAILURE;
    }

    //if there is no newline the line was too long, so skip the rest of it
    if (strchr(temp, '\n') == NULL) {
        int extra = getchar();
        while (extra != '\n' && extra != EOF) {
            extra = getchar();
        }
    }

    temp[strcspn(temp, "\r\n")] = '\0';
    strncpy(buf, temp, (size_t)size - 1);
    buf[size - 1] = '\0';

    return SUCCESS;
}


//
// FUNCTION    : getValidString
// DESCRIPTION :
//   This function reads a destination or a date from stdin. It keeps asking
//   until the string is not empty and is short enough to fit in buf.
// PARAMETERS  :
//   char* buf : where the string is stored
//   int size  : size of buf including the null terminator
// RETURNS     :
//   int : SUCCESS if a string was read, FAILURE if there is no more input
//
int getValidString(char* buf, int size) {
    char temp[BUFFER_SIZE] = "";
    int valid = 0;

    while (valid == 0) {
        if (getString(temp, sizeof(temp)) == FAILURE) {
            return FAILURE;
        }

        if (strlen(temp) == 0 || strlen(temp) >= (size_t)size) {
            printf("Invalid input, it must be 1 to %d characters. Enter it again: ", size - 1);
        }
        else {
            valid = 1;
        }
    }
    strcpy(buf, temp);

    return SUCCESS;
}


//
// FUNCTION    : getFare
// DESCRIPTION :
//   This function reads a fare from stdin as a floating-point number. It
//   keeps asking until the user enters a number that is not negative with
//   nothing else after it. A $ in front of it is allowed.
// PARAMETERS  :
//   float* fare : where the fare is stored
// RETURNS     :
//   int : SUCCESS if a fare was read, FAILURE if there is no more input
//
int getFare(float* fare) {
    char buf[BUFFER_SIZE] = "";
    char extra = 0;
    int valid = 0;

    while (valid == 0) {
        if (getString(buf, sizeof(buf)) == FAILURE) {
            return FAILURE;
        }

        //skip the $ if the user typed it
        char* number = buf;
        if (*number == '$') {
            number++;
        }

        //it has to start with a digit and have nothing left after the number
        if (isdigit((unsigned char)*number) && sscanf(number, "%f %c", fare, &extra) == 1) {
            valid = 1;
        }
        else {
            printf("Invalid fare, it must be a number like 250.75. Enter the fare again: ");
        }
    }
    return SUCCESS;
}
//
// FUNCTION    : createNewNode
// DESCRIPTION :
//   This function dynamically allocates the memory for a flight and creates a
//   new flight NODE. The destination and date are allocated to the exact size
//   of the strings. It is the calling function's responsibility to set the
//   PreviousFlight and NextFlight links.
// PARAMETERS  :
//   char* destination : destination of the flight
//   char* date        : date of the flight
//   float fare        : fare of the flight
// RETURNS     :
//   FlightNode* : pointer to the new node, or NULL if there is no memory
//
FlightNode* createNewNode(char* destination, char* date, float fare) {
    FlightNode* newFlight = (FlightNode*)malloc(sizeof(FlightNode));

    if (newFlight == NULL) {
        printf("No Memory!\n");
        return NULL;
    }

    newFlight->Flight.Destination = (char*)malloc(strlen(destination) + 1);
    newFlight->Flight.Date = (char*)malloc(strlen(date) + 1);

    if (newFlight->Flight.Destination == NULL || newFlight->Flight.Date == NULL) {
        printf("No Memory!\n");
        free(newFlight->Flight.Destination);
        free(newFlight->Flight.Date);
        free(newFlight);
        return NULL;
    }

    strcpy(newFlight->Flight.Destination, destination);
    strcpy(newFlight->Flight.Date, date);
    newFlight->Flight.Fare = fare;
    newFlight->PreviousFlight = NULL;
    newFlight->NextFlight = NULL;
    return newFlight;
}


//
// FUNCTION    : insertNewHead
// DESCRIPTION :
//   This function adds a node at the beginning of the list and shifts the
//   head to it. If the list is empty the node is also the tail.
// PARAMETERS  :
//   FlightNode** head     : pointer to head of list
//   FlightNode** tail     : pointer to tail of list
//   FlightNode* newFlight : node to add
// RETURNS     :
//   none
//
void insertNewHead(FlightNode** head, FlightNode** tail, FlightNode* newFlight) {
    if (*head == NULL) {
        *tail = newFlight;
        *head = newFlight;
    }
    else {
        (*head)->PreviousFlight = newFlight;
        newFlight->NextFlight = *head;
        *head = newFlight;
    }
}


//
// FUNCTION    : insertAfter
// DESCRIPTION :
//   This function adds a node right after the current node. If the current
//   node is the tail, the new node becomes the new tail.
// PARAMETERS  :
//   FlightNode** tail     : pointer to tail of list
//   FlightNode* current   : node that the new node is added after
//   FlightNode* newFlight : node to add
// RETURNS     :
//   none
//
void insertAfter(FlightNode** tail, FlightNode* current, FlightNode* newFlight) {
    newFlight->NextFlight = current->NextFlight;
    newFlight->PreviousFlight = current;

    if (current->NextFlight != NULL) {
        current->NextFlight->PreviousFlight = newFlight;
    }
    else {
        *tail = newFlight;
    }
    current->NextFlight = newFlight;
}


//
// FUNCTION    : insertSortedByFare
// DESCRIPTION :
//   This function creates a new flight and puts it in its sorted place at the
//   time of insertion. Sorting is done based on the fare (ascending). If two
//   flights have the same fare, the new one goes after the older one.
// PARAMETERS  :
//   FlightNode** head : pointer to head of list
//   FlightNode** tail : pointer to tail of list
//   char* destination : destination of the flight
//   char* date        : date of the flight
//   float fare        : fare of the flight
// RETURNS     :
//   int : SUCCESS if the flight was added, FAILURE if there is no memory
//
int insertSortedByFare(FlightNode** head, FlightNode** tail, char* destination, char* date, float fare) {
    FlightNode* newFlight = createNewNode(destination, date, fare);
    if (newFlight == NULL) {
        return FAILURE;
    }

    //Check if list is empty or the new fare is smaller than the head
    if (*head == NULL || fare < (*head)->Flight.Fare) {
        insertNewHead(head, tail, newFlight);
    }
    else {
        FlightNode* current = *head;

        // ITERATE TO FIND THE LAST ELEMENT THAT IS NOT BIGGER THAN THE NEW FARE
        while (current->NextFlight != NULL && current->NextFlight->Flight.Fare <= fare) {
            current = current->NextFlight;
        }
        insertAfter(tail, current, newFlight);
    }
    return SUCCESS;
}


//
// FUNCTION    : insertSortedByDestination
// DESCRIPTION :
//   This function creates a new flight and puts it in its sorted place at the
//   time of insertion. Sorting is done based on the destination (ascending).
//   If two flights have the same destination, the new one goes after the
//   older one.
// PARAMETERS  :
//   FlightNode** head : pointer to head of list
//   FlightNode** tail : pointer to tail of list
//   char* destination : destination of the flight
//   char* date        : date of the flight
//   float fare        : fare of the flight
// RETURNS     :
//   int : SUCCESS if the flight was added, FAILURE if there is no memory
//
int insertSortedByDestination(FlightNode** head, FlightNode** tail, char* destination, char* date, float fare) {
    FlightNode* newFlight = createNewNode(destination, date, fare);
    if (newFlight == NULL) {
        return FAILURE;
    }

    //Check if list is empty or the new destination comes before the head
    if (*head == NULL || strcmp(destination, (*head)->Flight.Destination) < 0) {
        insertNewHead(head, tail, newFlight);
    }
    else {
        FlightNode* current = *head;

        // ITERATE TO FIND THE LAST ELEMENT THAT DOES NOT COME AFTER THE NEW DESTINATION
        while (current->NextFlight != NULL &&
            strcmp(current->NextFlight->Flight.Destination, destination) <= 0) {
            current = current->NextFlight;
        }
        insertAfter(tail, current, newFlight);
    }
    return SUCCESS;
}
