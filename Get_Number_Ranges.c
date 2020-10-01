// Created by Noel Willems for COS326 Assignment 1: Number Range.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>

// Struct for number - includes numeric value and pointer to next
struct range {
	int start;
	int end;
	struct range *next;
} range;

// Get the starting/ending point of range
long getNums(char* line, long *n1, long *n2) {
    long num1;
	long num2;
	char* end;

    // First number
    num1 = strtol(line, &end, 10);

	// Check to see if there are more nums besides the first one
	int moreNums = 0;
	for(int i = 0; i < strlen(end); i++) {
		if(isdigit(end[i])) {
			moreNums = 1;
		}
	}

	// If there are more than 1 nums, get second num
	if(moreNums) {
   		num2 = strtol(end, &end, 10);
		*n1 = num1;
		*n2 = num2;
	// If there is only 1 num, range is just num1.
	} else {
		*n1 = num1;
		*n2 = num1;
	}
	return 0;
}

struct range* getSorted(struct range* r) {
	if(r == NULL) {
		return NULL;
	}
	int length = 0;
	struct range *test = r;
	while(test != NULL) {
		length = length + 1;	
		test = test->next;
	}

	int temp1;
	int temp2;
	for(int i = 0; i < length - 1; i++) {
		test = r;
		for(int j = 0; j < length - 1; j++) {
			if(test->start > test->next->start) {
				temp1 = test->start;
				temp2 = test->end;
				test->start = test->next->start;
				test->end = test->next->end;
				test->next->start = temp1;
				test->next->end = temp2;
			}
			test = test->next;
		}
	}
	return r;
}

int main(int argc, char* argv[]) {
	// Head of LL
	struct range *head = NULL;
    // Input file
    FILE * inputFile = fopen(argv[1], "r");

    // If input file fails
    if (inputFile == NULL) {
		perror("Error opening file");
		return (-1);
	}
    // Initializing line
    char line[256];
    // For getting rid of newlines
	char* pos;

	while (fgets(line, sizeof(line), inputFile)) {
		// Null-terminating the line
		line[strlen(line)] = '\0';

        // For newline chars
		if ((pos = strchr(line, '\n')) != NULL) {
			*pos = '\0';
		}

        // For carriage returns
		if ((pos = strchr(line, '\r')) != NULL) {
			*pos = '\0';
		}
		
		// Boolean for checking 
		int ok = 1;
		
		// For null lines
		if(line[0] == '\0') {
			ok = 0;
		}

        // For alphabetical characters
		for(int i = 0; i < strlen(line); i++) {
			if(isalpha(line[i])) {
				ok = 0;
				break;
			}
			if(line[i] == '/') {
				ok = 0;
				break;
			}
		}

		// If neither a comment or contains alphabet
        if((ok == 1)) {
			long num1, num2;
            getNums(line, &num1, &num2);
			int n1 = (int) num1;
			int n2 = (int) num2;
			// Get starting point and ending point
			int lg, sm;
			if(n1 > n2) { lg = n1; sm = n2; } else { lg = n2; sm = n1; }
			// Creating range Node for this line
			struct range *temp = NULL;
			// Create temporary range for sm and lg
			temp = (struct range*)malloc(sizeof(struct range));
			// Assign start data to smaller int and end data to larger int
			temp->start = sm;
			temp->end = lg;
			temp->next = NULL;
			// If the head of main's LL is null, set head to the newly created range
			if(head == NULL) {
				head = temp;
			// If there is already stuff in the LL
			} else {
				temp->next = head;
				head = temp;
			}
        }
	}	

	//Sorts from least to greatest
	getSorted(head);
	
	struct range *curr = head;
	int floor = curr->start;
	int floorsEnd = curr->end;	

	while(curr != NULL) {
		if(curr->next != NULL) {
			if(floor >= curr->next->start) {				
				floor = curr->next->start;
				floorsEnd = curr->next->end;
			} 
		}
		curr = curr->next;
	}
	struct range *newLL = (struct range*)malloc(sizeof(struct range));
	newLL-> start = floor;
	newLL -> end = floorsEnd;
	struct range *temp = head;
	struct range *tempNew;

	while(temp != NULL) {
		int done = 0;
		tempNew = newLL;
		while(tempNew != NULL) {
			// Case A: temp IS floor 
			if((temp->start == tempNew->start) && (temp->end == tempNew->end)) {
				done = 1;
			}
			// Case B: temp's first number is within new LL
			if((temp->start >= tempNew->start) && (temp->start <= tempNew->end + 1)) {
				if(temp-> end > tempNew->end) {
					tempNew->end = temp->end;
				}
				done = 1;
			}

			if(tempNew->next != NULL) {
				tempNew = tempNew->next;
			} else {
				break;
			}
		}
		// Case C: temp's first number is larger than new LL's end, create a new Node
		if(!done) {
			// Create new Node 
			struct range *newRange = (struct range*)malloc(sizeof(struct range));
			newRange->start = temp->start;
			newRange->end = temp->end;
			tempNew->next = newRange;
		}
		temp = temp->next; // Pointer for OG LL	
	}

	// Test print
	struct range *test1 = newLL;
	printf("\nNumber Range: \n");
	while(test1 != NULL) {
		if(test1->next == NULL) {
			if(test1->start == test1->end) {
				printf("%d\n", test1->start);
			} else {
				printf("%d - %d\n", test1->start, test1->end);
			}
		} else {
			if(test1->start == test1->end) {
				printf("%d, ", test1->start);
			} else {
				printf("%d - %d, ", test1->start, test1->end);
			}
		}	
		test1 = test1->next;
	}

	test1 = newLL;
	struct range *test2 = test1;
	// Received advice from Caleb in regards to how to free my mallocs.
	while(test1 != NULL) {		
		test1 = test1->next;
		free(test2);
		test2 = test1;
	}

	test1 = head;
	test2 = test1;
	// Received advice from Caleb in regards to how to free my mallocs.
	while(test1 != NULL) {		
		test1 = test1->next;
		free(test2);
		test2 = test1;
	}
	fclose(inputFile);
}
