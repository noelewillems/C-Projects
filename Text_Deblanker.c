/*
 Created by Noel Willems for COS326 Lab 3: deblank
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void deblank(char* str) {
	char *pnt = str;
	int x = strlen(pnt);
	for (int i = 0; i < x; i++) {
		if(*pnt == '\0') {
			break;
		}
		if (*pnt == ' ') {
			printf("%c", *pnt);
			while (*pnt == ' ') {
				pnt++;
			}
		}
		printf("%c", *pnt);
		pnt++;
	}
}

int main(int argc, char* argv[]) {
	char input[512];
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

		deblank(line);
		printf("\n");
	}
}

