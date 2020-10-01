// Created by Noel Willems for COS326 Lab

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#define MAX_LEN 64
#define NUM_WATCHES 9
#define FALSE 0
#define TRUE !(FALSE)
#define NANO_PER_SEC 1000000000.0

// Struct for watch
typedef struct watch {
   int id;
   int stop_flag;
   int status_flag;
   pthread_t thr;
} Watch;

// Method "watch_run" called in each stopwatch thread
void *watch_run(void *w) {
    // Get starting time
    struct timespec start; 
    clock_gettime(CLOCK_REALTIME, &start); 
    return start;
    printf("Starting stopwatch\n");
}

int main() {
    Watch *stop_watch[NUM_WATCHES];

    // Fills in watch 9 times
    for(int i=0; i<NUM_WATCHES; i++) { 
        stop_watch[i] = malloc(sizeof(Watch));
        stop_watch[i] -> id = i;
        stop_watch[i] -> stop_flag = 0;
        stop_watch[i] -> status_flag = 0;
    }

    // Boolean for when to quit
    int quit = FALSE;
    // Cmd line arg
    char cmd[MAX_LEN]; 
    // Watch number counter
    int cnt = 0;
 
    // While we aren't supposed to quit
    while(!quit) {
        printf(">");
        // Get user input cmd
        fgets(cmd, sizeof cmd, stdin);
        char *choice = strtok(cmd," \n"); 
        // if user says exit, quit
        if(strncmp("exit",choice,sizeof cmd)==0) {
            quit = TRUE;
        // If user says start, get len of time (watch_num)
        } else if(strncmp("start",choice,sizeof cmd)==0) {
            printf("Start\n");
            char *watch_num_str = strtok(NULL," \n"); 
            int watch_num;
            sscanf(watch_num_str,"%d",&watch_num);
            // Check if between 1-9
            if(watch_num < 1 || watch_num > 9) {
                printf("Invalid watch ID!\n");
            } else {
                if(stop_watch[watch_num]->stop_flag == 0) {
                    // stop_flag == 1 means that it has started
                    stop_watch[watch_num]->stop_flag = 1;
                    pthread_t watchThread;
                    stop_watch[watch_num]->thr = watchThread;
                    pthread_create(&watchThread, NULL, watch_run, &watch_num);
                }
            }

        // If user says stop
        } else if(strncmp("stop",choice,sizeof cmd)==0) {
            printf("stop\n");
            char *watch_num_str = strtok(NULL," \n"); 
            int watch_num;
            sscanf(watch_num_str,"%d",&watch_num);
            // Find stopwatch that needs to be stopped
            if(stop_watch[watch_num] -> stop_flag == 0) {
                printf("This stopwatch is already stopped.\n");
            } else {
                if(stop_watch[watch_num] -> stop_flag == 1) {
                    printf("Stopping...\n");
                    stop_watch[watch_num] -> stop_flag = 0;
                    pthread_join(stop_watch[watch_num]->thr, NULL);
                }
            }
        // If user asks for status
        } else if(strncmp("status",choice,sizeof cmd)==0) {
            char *watch_num_str = strtok(NULL," \n"); 
            int watch_num;
            sscanf(watch_num_str,"%d",&watch_num);
            if(stop_watch[watch_num] -> stop_flag == 0) {
                printf("This stopwatch is not currently running.\n");
            } else if(stop_watch[watch_num] -> stop_flag == 1) {
                struct timespec end; 
                clock_gettime(CLOCK_REALTIME, &end); 
                struct timespec *t;
                // Received advice from Caleb on how to return timespec from the run method
                pthread_join(stop_watch[watch_num]->thr, (void**)&t);
                printf("Time elapsed on this stopwatch: %ld\n", t->tv_sec);
            }
        } else {
            printf("%s is not a valid choice\n",choice);
        }
        cnt++;
    }
}