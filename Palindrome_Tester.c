/*
 Created by Noel Willems for COS326 Lab 3. Palindrome tester
 Received help from Caleb Hummel on checking for alphabetical chars and stuff.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int is_palindrome(char* string) {
	char* newstring = malloc(strlen(string) * sizeof(char));
	
	int len = strlen(string);
	int curr = 0;
	
	for (int i = 0; i < len; i++) {
		if (isalpha(*(string + i))) {
			*(newstring + curr) = tolower(*(string + i));
			curr++;
		} 
	}

	len = strlen(newstring);
	int beg = 0;
	int end = len - 1;
	int check = 1;
	for (int i = 0; i < len; i++) {
		if ((*(newstring + beg) == *(newstring + end))) {
			if (beg == end) {
				break;
			}
			end = end - 1;
			beg = beg + 1;
		} else {
			check = 0;
			break;
		}
	}
	free(newstring);
	return check;
}
int main(int argc, char* argv[]) {
	FILE * inputFile = fopen(argv[1], "r");
	if (inputFile == NULL) {
		perror("Error opening file");
		return (-1);
	}
	char line[256];
	char* pos;
	while (fgets(line, sizeof(line), inputFile)) {
		// Null-terminating the line
		line[strlen(line)] = '\0';

		if ((pos = strchr(line, '\n')) != NULL) {
			*pos = '\0';
		}
		if ((pos = strchr(line, '\r')) != NULL) {
			*pos = '\0';
		}
		is_palindrome(line);
		if ((is_palindrome(line)) == 0) {
			printf("%s is not a palindrome!\n", line);
		} else {
			printf("%s is a palindrome!\n", line);
		}
	}
	fclose(inputFile);
}
