// Created by Noel Willems
// Compile like: gcc CPU_Scheduler_Sim.c -o cpu_sim
// Run like: cpu_sim <process file name> <algorithm name>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

int numProcesses;  // num of total processes
int ticks = 0;     // ticks
int done = 0;      // 1 means all processes are done.
// Process
typedef struct Process {
    int id;               // id of process
    int cpu_time;         // time needed for CPU
    int io_time;          // time needed for I/O
    int reps;             // repetitions of process
    int priority;         // priority
    int end_time;         // when the process ends
    int wait_time;        // how long process spends in RTR queue
    int elapsed_io_time;  // time spent in IO
    int start_time;       // time started
} Process;

// Linked list of processes in the RTR "queue"
typedef struct Node {
    struct Node* next;  // pointer to next process
    Process* proc;      // "data" (pointer to Process struct)
} Node;

// Head of LL
Node* rtr_head;

// Head of IO LL
Node* io_head;

// Head of finished LL
Node* finished;
// Num in finished LL
int numFinished = 0;

// Algorithm identifier
int alg;

// Turnaround sum
int turnaround_sum;

// CPU struct: since the CPU can only contain ONE process, this is the only one.
typedef struct CPU {
    struct Node* cpu_process;  // the only process in the CPU at a given time
    int idle_time;             // time idle
    int busy_time;             // time busy
    int busy;                  // 0 means free; 1 means busy
} CPU;

void printRTR() {
    Node* curr;
    curr = rtr_head;
    while (curr != NULL) {
        printf("%d   ", curr->proc->id);
        curr = curr->next;
    }
    printf("\n");
}

void printIO() {
    Node* curr;
    curr = io_head;
    while (curr != NULL) {
        printf("%d ", curr->proc->id);
        curr = curr->next;
    }
}

void removeFromIO(Node* remove) {
    Node* curr = io_head;
    if (remove == io_head) {
        if (remove->next == NULL) {
            io_head = NULL;
        } else {
            io_head = remove->next;
            remove->next = NULL;
        }
        return;
    }
    while (curr->next != NULL) {
        if (curr->next->proc->id == remove->proc->id) {
            if (remove->next == NULL) {
                curr->next = NULL;
                remove->next = NULL;
            } else {
                curr->next = curr->next->next;
                remove->next = NULL;
            }
        }
    }
}
void increaseWait() {
    Node* curr;
    curr = rtr_head;
    while(curr != NULL) {
        curr->proc->wait_time++;
        curr = curr->next;
    }
}

void addToFinished(Node* fin) {
    fin->next = NULL;
    // Add process to list of IO LL
    Node* curr = finished;
    if (finished == NULL) {  // If the IO is empty
        finished = fin;
    } else {
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = fin;
    }
}

void addToIO(Node* p) {
    p->next = NULL;
    // Add process to list of IO LL
    Node* curr = io_head;
    if (io_head == NULL) {  // If the IO is empty
        io_head = p;
    } else {
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = p;
    }
}

// First come, first serve method : sort queue
void fcfs() {
    
}

// Priority scheduling method: order from lowest pri --> highest pri
void ps() {
    Node* curr = NULL;
    Node* temp = NULL;  // temporary Node
    Process* temp_proc =
        NULL;  // temporary Process pointer to process data of a Node
    curr = rtr_head;
    while (curr != NULL) {
        temp = rtr_head;
        while (temp->next != NULL) {
            if (temp->proc->priority >
                temp->next->proc->priority) {  // if the temp's priority is
                                               // 'higher' than next's priority
                temp_proc = temp->proc;  // set the temp_proc Process to temp's process
                temp->proc =temp->next ->proc;  // set temp's process to temp next's process
                temp->next->proc = temp_proc;  // set temp next's process to temp proc
            }
            temp = temp->next;
        }
        curr = curr->next;
    }
}

// Shortest job first method: order from shortest job length --> longest job (job length = cpu time * reps)
void sjf() {
    Node* curr = NULL;
    Node* temp = NULL;
    Process* temp_proc = NULL;
    curr = rtr_head;

    while (curr != NULL) {
        temp = rtr_head;
        while (temp->next != NULL) {
            if ((temp->proc->cpu_time * temp->proc->reps) >
                (temp->next->proc->cpu_time * temp->next->proc->reps)) {
                temp_proc = temp->proc;
                temp->proc = temp->next->proc;
                temp->next->proc = temp_proc;
            }
            temp = temp->next;
        }
        curr = curr->next;
    }
}

// Round robin method
void rr() { printf("round robin method\n"); }

void IO() {
    // Check every node in IO
    Node* temp;
    temp = io_head;
    while(temp != NULL) {
        // Check if the IO process is done, or had none to begin with.
        if(temp->proc->elapsed_io_time == temp->proc->io_time) {
            // Check if there are more reps.
            // If there are more reps, add to RTR.
            if(temp->proc->reps > 0) {
                Node* putBack = rtr_head;
                if(putBack == NULL) {
                //    printf("putting back to rtr, id of temp is %d\n", temp->proc->id);
                    temp->proc->elapsed_io_time = 0;
                    rtr_head = temp;
                } else {
                    while (putBack->next != NULL) {
                        putBack = putBack->next;
                    }
                    temp->proc->elapsed_io_time = 0;
                    putBack->next = temp;
                }
                // Sort again
                if(alg == 1) {
                    fcfs();
                } else if(alg == 2) {
                    ps();
                } else if(alg == 3) {
                    sjf();
                } else if(alg == 4) {
                    rr();
                }
                removeFromIO(temp);
            // No more reps in that IO and this is its last step
            } else {
                turnaround_sum = turnaround_sum + ticks;
                temp->proc->end_time = ticks;
                addToFinished(temp);
                numFinished++;
                removeFromIO(temp);
            }
        // Else, IO process has stuff to do.
        } else {
            temp->proc->elapsed_io_time++;
        }
        temp = temp->next;
    }
}


void run() {
    // Initializing 
    // Create and initialize the CPU.
    CPU* cpu = malloc(sizeof(CPU*));
    cpu->busy_time = 0;
    cpu->idle_time = 0;
    cpu->busy = 0;

    // Initializing head of finished LL
    finished = NULL;

    // Creating and initializing IO
    // Head of IO LL
    Node* io_head = NULL;

    // For statistics
    turnaround_sum = 0;  // sum of turnaround times
    int elapsed_cpu_time = 0;
    int elapsed_io_time = 0;

    // For iterating through RTR List
    Node* curr;
    printf("----------------------------------------\n");
    printf("| Tick     | CPU Process ID   | IO     |\n");
    printf("----------------------------------------\n");

    // Begin running the whole program.
    while (done == 0) {
        if (rtr_head != NULL && cpu->busy == 0) {
            curr = rtr_head;
            if(curr->proc->start_time == -1) {
                printf("if1\n");
                curr->proc->start_time = ticks;                    
            }

            rtr_head = rtr_head->next;  // "Pop off" the head of rtr and make it curr.
            curr->next = NULL;
            cpu->cpu_process = curr;
            elapsed_cpu_time = 0;
            cpu->busy = 1;
        }
        // If CPU is busy
        if (cpu->busy == 1) {
            // Check if process is done running. If it is, clear the CPU then see if you can send the process to IO.
            if (elapsed_cpu_time == cpu->cpu_process->proc->cpu_time) {
                // After process is done, reduce its number of reps and "free" the CPU.
                curr->proc->reps--;
                cpu->busy = 0;
                addToIO(cpu->cpu_process);
                // If the process in the CPU is not done running, keep elapsing CPU time.
            } else {
                elapsed_cpu_time++;
            }
        }
        increaseWait();
        IO();

        // Increment idle time and busy time
        if (cpu->busy == 0) {
            cpu->idle_time++;
        } else if (cpu->busy == 1) {
            cpu->busy_time++;
        }

        // Increment ticks
        ticks++;
        if(numProcesses == numFinished) {
            done = 1;
        }
    
        // Printing: if busy, print cpu process; if not busy, print xx
        if (cpu->busy == 1) {
            printf("|%9d |%18d|", ticks, cpu->cpu_process->proc->id);
            printIO();
            printf("\n");
        } else if (cpu->busy == 0) {
            printf("|%9d |%18c|", ticks, ' ');
            printIO();
            printf("\n");
        }
    }
    printf("------------------------------------\n");
    int average_turnaround = turnaround_sum / numProcesses;
    printf("Average turnaround time: %d\n", average_turnaround);
    printf("Busy CPU time: %d\n", cpu->busy_time);
    printf("Idle CPU time: %d\n", cpu->idle_time);

    Node* stats;
    stats = finished;
    while (stats != NULL) {
        printf("\nProcess ID: %d\n", stats->proc->id);
        printf("Start Time: %d\n", stats->proc->start_time);
        printf("End Time: %d\n", stats->proc->end_time);
        printf("Wait Time in RTR: %d\n", stats->proc->wait_time);
        stats = stats->next;
    }
    free(cpu);
    Node* fr;
    Process* frp;
    while(finished != NULL) {
        fr = finished;
        frp = finished->proc;
        finished = finished->next;
        free(frp);
        free(fr);
    }
}


int main(int argc, char* argv[]) {
    Node* head = NULL;  // initializing head of LL
    alg = 0; // initialize alg

    // Make sure the user inputs the correct number of args. If not, make suggestion & exit.
    if (argc != 3) {
        printf("Please enter args like: <program name> <process file name> <algorithm name>\n");
        exit(0);
    }

    char* proc_file_name = argv[1];  // process file name
    char* algorithm_name = argv[2];  // algorithm name
    char* allTokens[10];             // all tokens that can be on a line

    // Processing input file - if error, output message and return.
    FILE* inputFile = fopen(proc_file_name, "r");  // file of list of processes
    if (inputFile == NULL) {
        perror("Error opening file");
        return (-1);
    }

    int procCount = 0;
    char line[128];  // initialize line to array of chars
    char* pos;       // for getting rid of newline chars

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
        if ((line[0] == '/') || line[0] == '\0') {
            // Creating the processes, assigning their values, and adding them
            // to the "queue"
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
            while (token != NULL) {
                if (track == 1) {
                    proc->cpu_time = atoi(token);
                }
                if (track == 2) {
                    proc->io_time = atoi(token);
                }
                if (track == 3) {
                    proc->reps = atoi(token);
                }
                if (track == 4) {
                    proc->priority = atoi(token);
                }
                // Initializing other data
                proc->end_time = 0;
                proc->wait_time = 0;
                proc->start_time = -1;
                proc->elapsed_io_time = 0;
                track++;
                token = strtok(NULL, " ");
            }
            // Make a new node with the data of the newly made process
            Node* newNode = (Node*)malloc(sizeof(Node));
            // New node's process is the process
            newNode->proc = proc;
            // If the LL is empty, set head to new Node
            if (head == NULL) {
                head = newNode;
                rtr_head = head;
                head->next = NULL;
                // If LL has stuff in it, link the curr node to the new node
            } else {
                curr = head;
                while (curr->next != NULL) {
                    curr = curr->next;
                }
                curr->next = newNode;
            }
            procCount++;
        }
    }

    numProcesses = procCount;

    // Setting "alg" to different values for re-sorting
    if (strcasecmp(algorithm_name, "FCFS") == 0) {
        fcfs();
        alg = 1;
        run();
    } else if (strcasecmp(algorithm_name, "PS") == 0) {
        ps();
        alg = 2;
        run();
    } else if (strcasecmp(algorithm_name, "SJF") == 0) {
        sjf();
        alg = 3;
        run();
    } else if (strcasecmp(algorithm_name, "RR") == 0) {
        rr();
        alg = 4;
        run();
    }
    fclose(inputFile);
}