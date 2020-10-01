// Created by Noel Willems for COS326 Lab

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#define NANO_PER_SEC 1000000000.0
#define MAX_VAL 100
#define FALSE 0
#define TRUE !(FALSE)
int goal;
/* this is the method that will run in the thread */
void* make_guess(void *not_used) {  
    char guess_str[64];   
    int guess;   
    
    while(TRUE) {      
        printf("Guess a number between 1 and 100: ");      
        fgets(guess_str, sizeof guess_str, stdin);      
        sscanf(guess_str, "%d",  &guess); 
        if(goal == guess) {
            printf("You guessed it! Yay!\n");
            exit(1);
        } else if(goal > guess) {
            printf("Higher! ");
        } else if(goal < guess) {
            printf("Lower! ");
        }
    }   
    return not_used;
}

int main(int argc, char *argv[]) {   
    if(argc != 2) {      
        printf("Invalid command, should be hilo <seconds to timeout>\n");      
        exit(1);   
    }   

    struct timespec start;   
    struct timespec end;   
    double start_sec, end_sec, elapsed_sec;   
    clock_gettime(CLOCK_REALTIME, &start);   
    start_sec = start.tv_sec + start.tv_nsec/NANO_PER_SEC;
    
    // How long the user has to guess the rand number
    int timeout;   
    sscanf(argv[1], "%d", &timeout);
    // Creating rand int up to 100
    srand(time(NULL));
    goal = rand() % 100;
    printf("goal: %d\n", goal);

    /* create a thread will will ask user for guesses and check to see if they guessed right */
    pthread_t guessThr;
    // Thread is called guessThr, no special attributes, function pointer, pointer to args
    pthread_create(&guessThr, NULL, make_guess, &goal);

    start.tv_sec = 0;
    start.tv_nsec = 0;
    end.tv_sec = goal;

    struct timespec ts;
    if(clock_gettime(CLOCK_REALTIME, &ts) == -1){
        printf("clock error\n");
        exit(1);
    }
    ts.tv_sec+=timeout;
    int status = pthread_timedjoin_np(guessThr, NULL, &ts);
    // sleep(5);
    clock_gettime(CLOCK_REALTIME, &end);   
    end_sec = end.tv_sec + end.tv_nsec/NANO_PER_SEC;   
    elapsed_sec = end_sec - start_sec;   
   // printf("\nElapsed time %2.5f seconds\n", elapsed_sec);
    printf("You lose!\n");
    return 0;
} 
