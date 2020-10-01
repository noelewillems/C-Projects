/*
 Created by Noel Willems for COS 326 Lab 3: reverse string
*/
#include <stdio.h>
#include <string.h>

void reverse_string(char *string) {
	int len = strlen(string);
	for(int i = len - 1; i >= 0; i--) {
		printf("%c", *(string + i));
	}
	printf("\n");
}

int main(int argc, char* argv[]) {
	char* str = argv[1];
	reverse_string(str);
}
