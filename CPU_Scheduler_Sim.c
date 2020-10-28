// Created by Noel Willems
// Compile like: gcc CPU_Scheduler_Sim.c -o cpu_sim
// Run like: cpu_sim <process file name> <algorithm name>

/*
1.  Input file processing: take in process file name.
    a. Ignore comments (line starts with "//")
    b. Every line is: <process id> <cpu burst> <io burst> <repetitions> <priority>
    c. Example: 1 100 0 1 1 = process 1 spends 100 ticks in the cpu, 0 in io, occurs only once, and has a priority of 1 (low priority = first)
2.  Make a struct for each process. Have the id, cpu time, io time, rep, and priority all be properties of the struct.
3.  Add processes to the array of waiting processes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

int idle_CPU_time;


typedef struct Process {
    int id; // id of process
    int cpu_time; // time needed for CPU
    int io_time; // time needed for I/O
    int reps; // repetitions of process
    int priority; // priority 
    int start_time; // when the process begins
    int end_time; // when the process ends
    int wait_time; // how long process spends in RTR queue
} Process;

Process* rtr; // ready to run "queue"

int main(int argc, char *argv[]) {
    // Malloc array of processes in rtr
    rtr = (Process*)malloc(sizeof(Process) * 256);
    // Make sure the user inputs the correct number of args. If not, make suggestion & exit.
    if(argc != 3) {
        printf("Please enter args like this: <program name> <process file name> <algorithm name>\n");
        exit(0);
    }

    char* proc_file_name = argv[1]; // process file name
    char* algorithm_name = argv[2]; // algorithm name
    char* allTokens[10];

    // Processing input file - if error, output message and return.
    FILE * inputFile = fopen(proc_file_name, "r"); // file of list of processes
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

        // Ignore comments and null lines.
        if((line[0] == '/') || line[0] == '\0') {

        // Creating the processes, assigning their values, and adding them to the "queue"
        } else {
            // Malloc individual process
            Process *proc = (Process*)malloc(sizeof(Process));
            // Send to method that processes line char by char
            char* token = strtok(line, " ");
            proc->id = atoi(token);
            printf("%s is the id\n", token);
            int track = 1;
            int procCount = 0;
            while(token != NULL) {
                if(track == 1) {
                    proc->cpu_time = atoi(token);
                    printf("%s is the cpu time\n", token);
                }
                if(track == 2) {
                    proc->io_time = atoi(token);
                    printf("%s is the io time\n", token);
                }
                if(track == 3) {
                    proc->reps = atoi(token);
                    printf("%s is the rep\n", token);
                }
                if(track == 4) {
                    proc->priority = atoi(token);
                    printf("%s is the pri\n", token);
                }
                track++;              
                token = strtok(NULL, " ");
            }
            rtr[procCount] = proc;
        }
    }
}