// Created by Noel Willems
// Compile like: gcc CPU_Scheduler_Sim.c -o cpu_sim
// Run like: cpu_sim <process file name> <algorithm name>

/*
IDEA:
1. Sort queue appropriately.
2. Send to a method that runs a while loop, going over CPU and IO.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

int numProcesses; // num of total processes
int ticks = 0; // ticks 
int done = 0; // 1 means all processes are done.
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
    printf("RTR: ");
    while(curr != NULL) {
        printf("%d   ", curr->proc->id);
        curr = curr->next;
    }
    printf("\n");
}

// // Head of IO LL
// Node* io_head;

// void addToIO(Node* proc) {
//     Node* p = io_head;
//     if(io_head == NULL) {
//         io_head = proc;
//     } else {
//         while(p->next != NULL) {
//             p = p->next;
//         }
//         p->next = proc;
//     }
// }

// // IO stuff
// void IO_stuff(Node* curr) {
//     printf("%d is doing io now for a burst of %d\n", curr->proc->id, curr->proc->io_time);
//     Node* p = io_head;
    
//     // Add process to list of IO LL
//     if(io_head == NULL) {
//         io_head = curr;
//     } else {
        
//         while(p->next != NULL) {
//             p = p->next;
//         }
//         p->next = curr;
//     }

//     Node* bob = io_head;
//     printf("io head: %d\n", io_head->proc->id);
//     while(bob != NULL) {
//     //    printf("%d\n", bob->proc->id);
//         bob = bob->next;
//     }
//     // Have process stay in IO_stuff for length of io_burst
//     // When io_burst is finished, if there are no repetitions left, it goes into finished queue. Else, goes back into RTR.
//     // When done with IO stuff, go back to queue.
//     Node* test = rtr_head;
//     while(test->next != NULL) {
//         test = test->next;
//     } 
//     test->next = curr;
//     curr->next = NULL;
// }

// CPU stuff
// void CPU_stuff(Node* curr, CPU* cpu) {     

//     cpu->cpu_process = curr;
//     // Increment CPU busy time every time a processes uses the CPU
//     for(int i = 1; i <= curr->proc->cpu_time; i++) {
//         cpu->busy_time++;
//         cpu->busy = 1;
//     }
//     cpu->busy = 0;
// }

void run() {
    printf("Run method\n");
    // Create and initialize the CPU.
    CPU* cpu = malloc(sizeof(CPU*));
    cpu->busy_time = 0;
    cpu->idle_time = 0;
    cpu->busy = 0;
    Node* curr; // current node pointer of the RTR queue
    curr = rtr_head; // initialize to beginning of sorted RTR queue
    rtr_head = rtr_head->next; // "pop off" curr fromt he list
    curr->next = NULL;
    printf("------------------------------------------\n");
    printf("| Tick     | CPU Process ID     | IO     |\n");
    printf("------------------------------------------\n");

    int elapsed_cpu_time= 0;

    // Begin running the whole program.
    while(done == 0) {  
        // If CPU is free, add in the next process.
        if(cpu->busy == 0) {
            cpu->cpu_process = curr; // CPU process is curr
            cpu->busy = 1; // CPU is now busy
            elapsed_cpu_time++; // Keep track of how long curr is in CPU
        }

        // If CPU is busy
        if(cpu->busy == 1) {
            // Check if process is done running
            if(elapsed_cpu_time == cpu->cpu_process->proc->cpu_time) {
                // After process is done, reduce its number of reps.
                curr->proc->reps--; 
                // If there are more repetitions, put back into the RTR and free the CPU.
                if(curr->proc->reps > 0) {
                    Node* putBack = rtr_head;
                    while(putBack->next != NULL) {
                        putBack = putBack->next;
                    } 
                    putBack->next = curr;
                    curr->next = NULL;  
                    cpu->busy = 0;
                //    cpu->cpu_process->proc = NULL;
                // If there are no more repetitions, put it in the finished queue and free the CPU.
                } else if(curr->proc->reps == 0) {                   
                //    cpu->cpu_process->proc = NULL;
                    cpu->busy = 0;
                }
            } else {
                elapsed_cpu_time++;
            }
        }

        printf("|        %d |                 %d |      xx |\n", ticks, cpu->cpu_process->proc->id);
        ticks++;
        // Check if there are any more processes in the RTR.
        if(rtr_head != NULL && cpu->busy == 0) {
            curr = rtr_head;
            rtr_head = rtr_head->next; // "Pop off" the head of rtr and make it curr.
            curr->next = NULL;
            cpu->cpu_process = curr;
            elapsed_cpu_time = 0;
        } else if(rtr_head == NULL) {
            printf("finished!\n");
            done = 1;
        }
    }
}

// First come, first serve method : sort queue
void fcfs() {
    printf("First come, first serve method\n");
    printRTR();
    run();
}

// Priority scheduling method: order from lowest pri --> highest pri
void ps() {
    Node* curr = NULL;
    Node* temp = NULL; // temporary Node
    Process* temp_proc = NULL; // temporary Process pointer to process data of a Node
    curr = rtr_head;
    while(curr != NULL) {
        temp = rtr_head;
        while(temp->next != NULL) {
            if(temp->proc->priority > temp->next->proc->priority) { // if the temp's priority is 'higher' than next's priority
                temp_proc = temp->proc; // set the temp_proc Process to temp's process
                temp->proc = temp->next->proc; // set temp's process to temp next's process
                temp->next->proc = temp_proc; // set temp next's process to temp proc
            }
            temp = temp->next;
        }
        curr = curr->next;
    }
    printRTR();
}

// Shortest job first method: order from shortest job length --> longest job length
// Job length = cpu time * reps
void sjf() {
    Node* curr = NULL; 
    Node* temp = NULL;
    Process* temp_proc = NULL;
    curr = rtr_head;

    while(curr != NULL) {
        temp = rtr_head;
        while(temp->next != NULL) {
            if((temp->proc->cpu_time * temp->proc->reps) > (temp->next->proc->cpu_time * temp->next->proc->reps)) {
                temp_proc = temp->proc;
                temp->proc = temp->next->proc;
                temp->next->proc = temp_proc;
            } 
            temp = temp->next;
        }
        curr = curr->next;
    }
    printRTR();
}

// Round robin method
void rr() {
    printf("round robin method\n");
}

int main(int argc, char *argv[]) {
    Node* head = NULL; // initializing head of LL
//    io_head = NULL;
    // CPU
    CPU* cpu = malloc(sizeof(CPU*));
//    cpu->busy_time = 0;
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