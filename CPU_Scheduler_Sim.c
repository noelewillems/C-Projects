// Created by Noel Willems
// Compile like: gcc CPU_Scheduler_Sim.c -o cpu_sim
// Run like: cpu_sim <process file name> <algorithm name>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

int numProcesses; // num of total processes
int ticks = 0;

// Process
typedef struct Process {
    int id; // id of process
    int cpu_time; // time needed for CPU
    int io_time; // time needed for I/O
    int reps; // repetitions of process
    int priority; // priority 
    int start_time; // when the process begins
    int end_time; // when the process ends
    int wait_time; // how long process spends in RTR queue
    int currTime; // time spent in a certain state
} Process;

// Linked list of processes in the RTR "queue"
typedef struct Node {
    struct Node* next; // pointer to next process
    Process* proc; // "data" (pointer to Process struct)
} Node;

// Head of LL
Node* rtr_head;

// CPU struct: since the CPU can only contain ONE process, this is the only one.
typedef struct CPU {
    struct Node* cpu_process; // the only process in the CPU at a given time
    int idle_time; // time idle
    int busy_time; // time busy
    int busy; // 0 means free; 1 means busy
} CPU;

void printRTR() {
    Node* curr;
    curr = rtr_head;
    while(curr != NULL) {
        printf("rtr: %d\n", curr->proc->id);
        curr = curr->next;
    }
}

// Head of IO LL
Node* io_head;

void addToIO(Node* proc) {
    Node* p = io_head;
    if(io_head == NULL) {
        io_head = proc;
    } else {
        while(p->next != NULL) {
            p = p->next;
        }
        p->next = proc;
    }
}

// IO stuff
void IO_stuff(Node* curr) {
    printf("curr: %d\n", curr->proc->id);
    Node* p = io_head;
    
    // Add process to list of IO LL
    if(io_head == NULL) {
        io_head = curr;
    } else {
        
        while(p->next != NULL) {
            p = p->next;
        }
        p->next = curr;
    }

    Node* bob = io_head;
    printf("io head: %d\n", io_head->proc->id);
    while(bob != NULL) {
    //    printf("%d\n", bob->proc->id);
        bob = bob->next;
    }
    // Have process stay in IO_stuff for length of io_burst
    // When io_burst is finished, if there are no repetitions left, it goes into finished queue. Else, goes back into RTR.
    // When done with IO stuff, go back to queue.
    Node* test = rtr_head;
    while(test->next != NULL) {
        test = test->next;
    } 
    test->next = curr;
    curr->next = NULL;
}

// CPU stuff
void CPU_stuff(Node* curr, CPU* cpu) {     

    cpu->cpu_process = curr;
    // Increment CPU busy time every time a processes uses the CPU
    for(int i = 1; i <= curr->proc->cpu_time; i++) {
        cpu->busy_time++;
        cpu->busy = 1;
    }
    cpu->busy = 0;
}


// First come, first serve method
void fcfs() {
    // CPU and initializations
    CPU* cpu = malloc(sizeof(CPU*));
    cpu->busy_time = 0;
    cpu->idle_time = 0;
    cpu->busy = 0;
    Node* curr; // current node pointer
    // curr = rtr_head;  
    // rtr_head = rtr_head->next; // new head of queue - curr is sent ot the CPU.
    // curr->next = NULL;
    // while(curr != NULL) {  
    //     if(cpu->busy = 0) {
    //         if(curr->proc->cpu_time>0) {
    //             CPU_stuff(curr, cpu);
    //         }
    //     }
    //     // Repeat for number of repetitions
    //     for(int i = 0; i < curr->proc->reps; i++) {
    //         if((curr->proc->cpu_time > 0) && (cpu->busy == 0)) {
    //             CPU_stuff(curr, cpu);
    //         } 
    //     }

    // }
    while(1) {
        // cpu: empty, full, process finished, process unfinished
        // If CPU is empty
        if(cpu->cpu_process == NULL) {
            curr = rtr_head; // new head of queue
            if(rtr_head != NULL) {
                rtr_head = rtr_head->next; // "popping off" the head of the rtr queue
                curr->next = NULL;
            }            
            cpu->cpu_process = curr;
        }

        // If CPU's process is finished
        if(cpu->cpu_process != NULL) {
            if(cpu->cpu_process->proc->cpu_time == cpu->cpu_process->proc->currTime) {
                // Remove from CPU, if there is IO time and more reps, do IO
                Node* finished_cpu_proc = cpu->cpu_process;
                if(cpu->cpu_process->proc->io_time > 0) {
                    addToIO(finished_cpu_proc);
                }
                cpu->cpu_process = NULL;
            }
        } else {
            cpu->idle_time++;
            printf("cpu idle time: %d\n", cpu->idle_time);
        }
    
        // If CPU has a process
        if(cpu->cpu_process != NULL) {
            cpu->busy++;
            cpu->cpu_process->proc->currTime++;
        }

        // If CPU's process is unfinished
        ticks++;
    }
    printf("cpu busy time: %d\n", cpu->busy_time);
}

// Priority scheduling method
void ps() {
    printf("priority scheduling method\n");
}

// Shortest job first method
void sjf() {
    printf("shortest job first method\n");
}

// Round robin method
void rr() {
    printf("round robin method\n");
}

int main(int argc, char *argv[]) {
    Node* head = NULL; // initializing head of LL
    io_head = NULL;
    // CPU
    CPU* cpu = malloc(sizeof(CPU*));
    cpu->busy_time = 0;
    // Make sure the user inputs the correct number of args. If not, make suggestion & exit.
    if(argc != 3) {
        printf("Please enter args like: <program name> <process file name> <algorithm name>\n");
        exit(0);
    }

    char* proc_file_name = argv[1]; // process file name
    char* algorithm_name = argv[2]; // algorithm name
    char* allTokens[10]; // all tokens that can be on a line

    // Processing input file - if error, output message and return.
    FILE * inputFile = fopen(proc_file_name, "r"); // file of list of processes
    if (inputFile == NULL) {
		perror("Error opening file");
		return (-1);
	}

    int procCount = 0;
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
            Process* proc = (Process*)malloc(sizeof(Process));
            // Send to method that processes line char by char
            char* token = strtok(line, " ");
            // Initialize id, cpu time, io time, repetitions, and priority
            proc->id = atoi(token);
            token = strtok(NULL, " ");
            int track = 1;
            // Current node pointer
            Node* curr;
            while(token != NULL) {
                if(track == 1) {
                    proc->cpu_time = atoi(token);
                }
                if(track == 2) {
                    proc->io_time = atoi(token);
                }
                if(track == 3) {
                    proc->reps = atoi(token);
                }
                if(track == 4) {
                    proc->priority = atoi(token);
                }
                // Set start time to 0, and initialize end time/wait time to 0
                proc->start_time = 0;
                proc->end_time = 0;
                proc->wait_time = 0;
                proc->currTime = 0;
                track++;              
                token = strtok(NULL, " ");
            }
            // Make a new node with the data of the newly made process
            Node* newNode = (Node*)malloc(sizeof(Node));
            // New node's process is the process
            newNode->proc = proc;
            // If the LL is empty, set head to new Node
            if(head == NULL) {
                head = newNode;
                rtr_head = head;
                head->next = NULL;
            // If LL has stuff in it, link the curr node to the new node
            } else {
                curr = head;
                while(curr->next != NULL) {
                    curr = curr->next;
                }
                curr->next = newNode;
            }
            procCount++;
        }
    }

    numProcesses = procCount;

    if(strcasecmp(algorithm_name, "FCFS") == 0) {
        fcfs();
    } else if(strcasecmp(algorithm_name, "PS") == 0) {
        ps();
    } else if(strcasecmp(algorithm_name, "SJF") == 0) {
        sjf();
    } else if(strcasecmp(algorithm_name, "RR") == 0) {
        rr();
    }
}