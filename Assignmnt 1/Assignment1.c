/*
* PROJECT       : Assignment 1 - Sorted Doubly-Linked Lists
* PROGRAMMER    : Danial Rezvanirashidabad
* DESCRIPTION   :
*   This program gets flight information (destination, date and fare) from the
*   user and stores every flight in two separate sorted doubly linked lists.
*   One list is sorted by fare and the other one is sorted by destination.
*   Every flight is put into its proper place at the time of insertion.
*   After the input is done both lists are printed. Then the user can search
*   for one flight by destination and date and change its fare. Both lists are
*   printed again and all of the memory is freed at the end.
*/

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DESTINATION_SIZE 15     //max destination length including the null terminator
#define DATE_SIZE 9             //max date length (DDMMYYYY) including the null terminator
#define BUFFER_SIZE 121         //size of the buffer used to read one line from stdin
#define STOP_INPUT "."          //destination or date that ends the flight input

//Information about one flight. Destination and Date are dynamically allocated
typedef struct FlightInfo {
    char* Destination;
    char* Date;
    float Fare;
} FlightInfo;

//One node of the doubly linked list
typedef struct FlightNode {
    FlightInfo Flight;
    struct FlightNode* PreviousFlight;
    struct FlightNode* NextFlight;
} FlightNode;

int getString(char* buf, int size);
int getValidString(char* buf, int size);
int getFare(float* fare);
FlightNode* CreateNewNode(char* destination, char* date, float fare);

void InsertNewHead(FlightNode** head, FlightNode** tail, FlightNode* newFlight);

void insertAfter(FlightNode** tail, FlightNode* current, FlightNode* newFlight);

int insertSortedByFare(FlightNode** head, FlightNode** tail, char* destination, char* date, float fare);

int compareDestination(char* first, char* second);

int insertSortedByDestination(FlightNode** head, FlightNode** tail, char* destination, char* date, float fare);

int getFlights(FlightNode** fareHead, FlightNode** fareTail, FlightNode** destinationHead, FlightNode** destinationTail);

void PrintList(FlightNode* head);
FlightNode* findFlight(FlightNode* head, char* destination, char* date);
void deleteNode(FlightNode* node, FlightNode** head, FlightNode** tail);
int changeFare(FlightNode** fareHead, FlightNode** fareTail, FlightNode* destinationHead);
void FreeList(FlightNode** head, FlightNode** tail);


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
//   int : 0 if a line was read, -1 if there is no more input
//
int getString(char* buf, int size) {
    char temp[BUFFER_SIZE] = "";

    buf[0] = '\0';

    if (fgets(temp, sizeof(temp), stdin) == NULL) {
        return -1;
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

    return 0;
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
//   int : 0 if a string was read, -1 if there is no more input
//
int getValidString(char* buf, int size) {
    char temp[BUFFER_SIZE] = "";
    int valid = 0;

    while (valid == 0) {
        if (getString(temp, sizeof(temp)) == -1) {
            return -1;
        }

        if (strlen(temp) == 0 || strlen(temp) >= (size_t)size) {
            printf("Invalid input, it must be 1 to %d characters. Enter it again: ", size - 1);
        }
        else {
            valid = 1;
        }
    }
    strcpy(buf, temp);

    return 0;
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
//   int : 0 if a fare was read, -1 if there is no more input
//
int getFare(float* fare) {
    char buf[BUFFER_SIZE] = "";
    char extra = 0;
    int valid = 0;

    while (valid == 0) {
        if (getString(buf, sizeof(buf)) == -1) {
            return -1;
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
    return 0;
}


//
// FUNCTION    : CreateNewNode
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
FlightNode* CreateNewNode(char* destination, char* date, float fare) {
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
// FUNCTION    : InsertNewHead
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
void InsertNewHead(FlightNode** head, FlightNode** tail, FlightNode* newFlight) {
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
//   int : 0 if the flight was added, -1 if there is no memory
//
int insertSortedByFare(FlightNode** head, FlightNode** tail, char* destination, char* date, float fare) {
    FlightNode* newFlight = CreateNewNode(destination, date, fare);
    if (newFlight == NULL) {
        return -1;
    }

    //Check if list is empty or the new fare is smaller than the head
    if (*head == NULL || fare < (*head)->Flight.Fare) {
        InsertNewHead(head, tail, newFlight);
    }
    else {
        FlightNode* current = *head;

        // ITERATE TO FIND THE LAST ELEMENT THAT IS NOT BIGGER THAN THE NEW FARE
        while (current->NextFlight != NULL && current->NextFlight->Flight.Fare <= fare) {
            current = current->NextFlight;
        }
        insertAfter(tail, current, newFlight);
    }
    return 0;
}


//
// FUNCTION    : compareDestination
// DESCRIPTION :
//   This function compares two destinations alphabetically without caring
//   about capital letters, so "miami" and "Miami" are sorted the same way.
//   (strcmp puts every capital letter before every small letter.)
// PARAMETERS  :
//   char* first  : first destination
//   char* second : second destination
// RETURNS     :
//   int : less than 0 if first comes before second, 0 if they are the same,
//         more than 0 if first comes after second
//
int compareDestination(char* first, char* second) {
    while (*first != '\0' && tolower((unsigned char)*first) == tolower((unsigned char)*second)) {
        first++;
        second++;
    }
    return tolower((unsigned char)*first) - tolower((unsigned char)*second);
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
//   int : 0 if the flight was added, -1 if there is no memory
//
int insertSortedByDestination(FlightNode** head, FlightNode** tail, char* destination, char* date, float fare) {
    FlightNode* newFlight = CreateNewNode(destination, date, fare);
    if (newFlight == NULL) {
        return -1;
    }

    //Check if list is empty or the new destination comes before the head
    if (*head == NULL || compareDestination(destination, (*head)->Flight.Destination) < 0) {
        InsertNewHead(head, tail, newFlight);
    }
    else {
        FlightNode* current = *head;

        // ITERATE TO FIND THE LAST ELEMENT THAT DOES NOT COME AFTER THE NEW DESTINATION
        while (current->NextFlight != NULL &&
            compareDestination(current->NextFlight->Flight.Destination, destination) <= 0) {
            current = current->NextFlight;
        }
        insertAfter(tail, current, newFlight);
    }
    return 0;
}


//
// FUNCTION    : getFlights
// DESCRIPTION :
//   This function reads the destination, date and fare of each flight from
//   stdin (each on a separate line) and adds the flight to both sorted lists.
//   It stops as soon as the user enters "." as the destination or the date,
//   and that flight is ignored. Invalid input is asked for again.
// PARAMETERS  :
//   FlightNode** fareHead        : pointer to head of the list sorted by fare
//   FlightNode** fareTail        : pointer to tail of the list sorted by fare
//   FlightNode** destinationHead : pointer to head of the list sorted by destination
//   FlightNode** destinationTail : pointer to tail of the list sorted by destination
// RETURNS     :
//   int : 0 if the input is done, -1 if there is no memory
//
int getFlights(FlightNode** fareHead, FlightNode** fareTail, FlightNode** destinationHead, FlightNode** destinationTail) {
    char destination[DESTINATION_SIZE] = "";
    char date[DATE_SIZE] = "";
    float fare = 0;
    int status = 0;
    int done = 0;

    while (done == 0) {
        printf("Enter the destination (. to stop): ");
        if (getValidString(destination, sizeof(destination)) == -1 || strcmp(destination, STOP_INPUT) == 0) {
            done = 1;
        }
        else {
            printf("Enter the date as DDMMYYYY (. to stop): ");
            if (getValidString(date, sizeof(date)) == -1 || strcmp(date, STOP_INPUT) == 0) {
                done = 1;
            }
            else {
                printf("Enter the fare: ");
                if (getFare(&fare) == -1) {
                    done = 1;
                }
                //add the flight to both lists
                else if (insertSortedByFare(fareHead, fareTail, destination, date, fare) == -1 ||
                    insertSortedByDestination(destinationHead, destinationTail, destination, date, fare) == -1) {
                    status = -1;
                    done = 1;
                }
            }
        }
    }
    return status;
}


//
// FUNCTION    : PrintList
// DESCRIPTION :
//   This function prints the destination, date and fare of every flight
//   within the linked list, one flight per line.
// PARAMETERS  :
//   FlightNode* head : head of list
// RETURNS     :
//   none
//
void PrintList(FlightNode* head) {
    FlightNode* current = head;
    while (current != NULL) {
        printf("%-15s%-10s%-.2f\n", current->Flight.Destination, current->Flight.Date, current->Flight.Fare);
        current = current->NextFlight;
    }
}


//
// FUNCTION    : findFlight
// DESCRIPTION :
//   This function iterates through the list and checks if both the
//   destination and the date of a node are equal to the given ones
//   (case-sensitive).
// PARAMETERS  :
//   FlightNode* head  : head of list
//   char* destination : pointer to null-terminated string containing destination
//   char* date        : pointer to null-terminated string containing date
// RETURNS     :
//   FlightNode* : pointer to the node containing the flight, or NULL if the
//                 flight is not found
//
FlightNode* findFlight(FlightNode* head, char* destination, char* date) {
    FlightNode* current = head;
    while (current != NULL) {
        if (strcmp(current->Flight.Destination, destination) == 0 &&
            strcmp(current->Flight.Date, date) == 0) {
            return current;
        }
        current = current->NextFlight;
    }
    return NULL;
}


//
// FUNCTION    : deleteNode
// DESCRIPTION :
//   This function deletes a node from the list. It relinks the pointers
//   around the node first (and moves the head or the tail if the node is one
//   of them) and then frees the memory of the node.
// PARAMETERS  :
//   FlightNode* node  : node to delete
//   FlightNode** head : pointer to head of list
//   FlightNode** tail : pointer to tail of list
// RETURNS     :
//   none
//
void deleteNode(FlightNode* node, FlightNode** head, FlightNode** tail) {
    if (node == NULL) {
        return;
    }

    if (node->PreviousFlight != NULL) {
        node->PreviousFlight->NextFlight = node->NextFlight;
    }
    else {
        //the node is the head
        *head = node->NextFlight;
    }

    if (node->NextFlight != NULL) {
        node->NextFlight->PreviousFlight = node->PreviousFlight;
    }
    else {
        //the node is the tail
        *tail = node->PreviousFlight;
    }

    free(node->Flight.Destination);
    free(node->Flight.Date);
    free(node);
}


//
// FUNCTION    : changeFare
// DESCRIPTION :
//   This function reads a destination and a date from stdin and searches for
//   that flight in the list sorted by destination. If the flight is found it
//   prints the fare and asks for a new fare. If the fare is changed, the list
//   sorted by destination is updated, and the flight is deleted from the list
//   sorted by fare and inserted again based on the new fare.
// PARAMETERS  :
//   FlightNode** fareHead       : pointer to head of the list sorted by fare
//   FlightNode** fareTail       : pointer to tail of the list sorted by fare
//   FlightNode* destinationHead : head of the list sorted by destination
// RETURNS     :
//   int : 0 normally, -1 if there is no memory
//
int changeFare(FlightNode** fareHead, FlightNode** fareTail, FlightNode* destinationHead) {
    char destination[DESTINATION_SIZE] = "";
    char date[DATE_SIZE] = "";
    int status = 0;

    printf("Enter the destination of the flight to search for: ");
    getString(destination, sizeof(destination));
    printf("Enter the date of the flight to search for (DDMMYYYY): ");
    getString(date, sizeof(date));

    FlightNode* found = findFlight(destinationHead, destination, date);

    if (found == NULL) {
        printf("Flight Not Found!\n");
    }
    else {
        float newFare = found->Flight.Fare;

        printf("Fare: %.2f\n", found->Flight.Fare);
        printf("Enter the new fare: ");

        if (getFare(&newFare) == -1 || newFare == found->Flight.Fare) {
            printf("Fare is unchanged\n");
        }
        else {
            //update the list sorted by destination
            found->Flight.Fare = newFare;

            //delete from the list sorted by fare and insert again at the new place
            deleteNode(findFlight(*fareHead, destination, date), fareHead, fareTail);
            status = insertSortedByFare(fareHead, fareTail, destination, date, newFare);
        }
    }
    return status;
}


//
// FUNCTION    : FreeList
// DESCRIPTION :
//   This function iterates over the entire list and frees up the memory
//   space. After it is done the list is empty.
// PARAMETERS  :
//   FlightNode** head : pointer to head of list
//   FlightNode** tail : pointer to tail of list
// RETURNS     :
//   none
//
void FreeList(FlightNode** head, FlightNode** tail) {
    while (*head != NULL) {
        deleteNode(*head, head, tail);
    }
}


int main(void) {
    //list sorted by fare, at first it is empty
    FlightNode* fareHead = NULL;
    FlightNode* fareTail = NULL;

    //list sorted by destination, at first it is empty
    FlightNode* destinationHead = NULL;
    FlightNode* destinationTail = NULL;

    //1. Get all the flights and add them to both lists
    int status = getFlights(&fareHead, &fareTail, &destinationHead, &destinationTail);

    if (status == 0) {
        //2. Print both lists
        printf("Flights sorted by fare:\n");
        PrintList(fareHead);
        printf("Flights sorted by destination:\n");
        PrintList(destinationHead);

        //3. Search for one flight and change its fare
        status = changeFare(&fareHead, &fareTail, destinationHead);
    }

    if (status == 0) {
        //4. Print both lists again
        printf("Flights sorted by fare:\n");
        PrintList(fareHead);
        printf("Flights sorted by destination:\n");
        PrintList(destinationHead);
    }

    //5. CLEAR OUT BOTH LISTS
    FreeList(&fareHead, &fareTail);
    FreeList(&destinationHead, &destinationTail);
    return status;
}