// Created by Noel Willems - a virtual memory manager simulation (paging)
// Run like: vmm InputFile.txt BACKING_STORE.bin

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Process the input file
void processInput(char* input_file_name) {
    // Processing input file - if error, output message and return.
    FILE* inputFile = fopen(input_file_name, "r"); 
    if (inputFile == NULL) {
        perror("Error opening file");
        exit(0);
    }
    char line[128];
    char* pos;
    while (fgets(line, sizeof(line), inputFile)) {
        int address = 0;
        int page = 0;
        int offset = 0;
        line[strlen(line)] = '\0'; // Null-terminating the line
        if ((pos = strchr(line, '\n')) != NULL) { *pos = '\0'; } // For newline chars
        if ((pos = strchr(line, '\r')) != NULL) { *pos = '\0'; } // For carriage returns
        if (line[0] == '\0') { // Ignore null lines
        } else {
            // Next 3 lines of code are provided by Dr. Hunt 
            address = atoi(line);
            page = (0x0000FF00 & address) >> 8;
            offset = 0x000000FF & address;
        }
        printf("For address %d, page is %d and offset is %d\n",address, page, offset);
    }
}

int main(int argc, char* argv[]) {
    // Read input file of InputFile.txt
    if(argc != 3) {
        printf("Please run like: ./vmm <input file name> <backing store bin file name>\n");
    }

    char* input_file_name = argv[1];
    processInput(input_file_name);
    char* backingStoreName = argv[2];
}