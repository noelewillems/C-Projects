// Created by Noel Willems
// Compile like: gcc CPU_Scheduler_Sim.c -o cpu_sim
// Run like: cpu_sim <process file name> <algorithm name>

/*
1.  Input file processing: take in process file name.
    a. Ignore comments (line starts with "//")
    b. Every line is: <process id> <cpu burst> <io burst> <repetitions> <priority>
    c. Example: 1 100 0 1 1 = process 1 spends 100 ticks in the cpu, 0 in io, occurs only once, and has a priority of 1 (low priority = first)
2.  Based on the algorithm name, send to a specific method.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Getting args from command line
    if(argc != 3) {
        printf("Please enter args like this: <program name> <process file name> <algorithm name>\n");
        exit(0);
    }

    char* procFileName = argv[1]; // process file name
    char* algorithmName = argv[2]; // algorithm name

    // Processing input file
    FILE * inputFile = fopen(procFileName, "r"); // file of list of processes
    if (inputFile == NULL) {
		perror("Error opening file");
		return (-1);
	}

    char line[128]; // initialize line to array of chars
    char *pos; // for getting rid of newline chars
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

        // For comments and null lines
        if((line[0] == '/') || line[0] == '\0') {

        } else {
            printf("line: %s\n", line);
        }
    }
}