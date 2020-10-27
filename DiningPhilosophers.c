#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define NUM 5

// The 2 forks that the philosophers have on their right and their left
int leftFork(int p) { return p; }
int rightFork(int p) { return (p+1) %5; }

// Array of fork semaphores
sem_t forks[NUM];
// Array of philosophers
pthread_t phils[NUM];

void eat(int id) {
    printf("Philosopher #%d is eating.\n", id);
}
void think(int id) {
    printf("Philosopher #%d is thinking.\n", id);
}

void *philosopher_stuff(void *id) {
    printf("philosopher_stuff\n");
    while(1) {
        int phil_id = *((int *) id);
        think(phil_id);
    //    pick_up_fork(phil_id); // p is the philosopher
        eat(phil_id);
    //    put_down_fork(phil_id); 
    }
}

int main(int argc, char *argv[]) {
    // Initialize array of fork semaphores
    for(int i = 0; i < NUM; i++) {
        // Initialize value to 0 because this is an ORDERING problem
        sem_init(&forks[i], 0, 0);
    }
    // Initialize array of philosophers and create threads for them
    for(int i = 0; i < NUM; i++) {
        int *phil_id = malloc(sizeof(int));
        *phil_id = i;
        phils[i] = pthread_create(&phils[i], NULL, philosopher_stuff, phil_id);
    }

    sleep(1);

    // Join threads
    for(int i = 0; i < NUM; i++) {
        printf("join\n");
        pthread_join(phils[i], NULL);
    }
}
