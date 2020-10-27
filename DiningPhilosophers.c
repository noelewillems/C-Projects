// Created by Noel Willems: Solving Dining Philosophers problem with semaphores.
// Compile like: gcc DiningPhilosophers.c -lpthread -o dp
// Run like: ./dp

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
int num;

typedef struct Philosopher {
    int id;
    int hungry; // 1 = hungry
    int thinking; // 1 = thinking
    int eating; // 1 = eating
} Philosopher;

// The 2 forks that the philosophers have on their right and their left
int leftFork(int p) { return p; }
int rightFork(int p) { return (p+1) %5; }

// Pointer to array of fork semaphores
sem_t* forks;
// Pointer to array of philosophers
pthread_t* phils;

void think(Philosopher* p) {
    // Think for 2 secs, then get hungry
    printf("Philosopher %d is thinking.\n", p->id);
    sleep(2);
    p->thinking = 0;
    printf("Philosopher %d is hungry.\n", p->id);
    p->hungry = 1;
}

void eat(Philosopher* p) {
    printf("Philosopher %d is eating.\n", p->id);
    sleep(1);
    sem_post(&forks[leftFork(p->id)]);
    sem_post(&forks[rightFork(p->id)]);
    printf("Philosopher %d is done eating.\n", p->id);
    p->hungry = 0;
    p->thinking = 1;
    think(p);
}

void hungry(Philosopher* p) {
    // If left AND right fork are free, grab them.
    int r;
    sem_getvalue(&forks[rightFork(p->id)], &r); // Value of right fork
    int l;
    sem_getvalue(&forks[leftFork(p->id)], &l); // Value of left fork

    if(r > 0 && l > 0) {
        printf("Philosopher %d has grabbed forks %d and %d.\n", p->id, leftFork(p->id), rightFork(p->id));
        sem_wait(&forks[leftFork(p->id)]);
        sem_wait(&forks[rightFork(p->id)]);
        eat(p);
    }
}

// Thread method
void *philosopher_stuff(void *ph) {
    Philosopher* phil = (Philosopher *) ph;
    while(1) {
        if(phil->thinking == 1) {
            think(phil);
        }

        if(phil->eating == 1) {
            printf("Philosopher %d is eating!\n", phil->id);
        }

        if(phil->hungry == 1) {
            hungry(phil);
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Please enter args like this: <program name> <philosophers count>\n");
        exit(0);
    } else {
        num = atoi(argv[1]);
    }

    forks = (sem_t*)malloc(sizeof(sem_t) * num);
    phils = (pthread_t*)malloc(sizeof(pthread_t) * num);
    printf("argc: %d\n", argc);
    // Initialize array of fork semaphores
    for(int i = 0; i < num; i++) {
        // Initialize value to 1 because we're using it like a lock (binary semaphore)
        sem_init(&forks[i], 0, 1);
    }
    // Initialize array of philosophers and create threads for them
    for(int i = 0; i < num; i++) {
        printf("i: %d\n", i);
        Philosopher *phil = (Philosopher*)malloc(sizeof(Philosopher));
        phil->id = i;
        phil->thinking = 1;
        phil->hungry = 0;
        phil->eating = 0;
        printf("creating thread %d\n", i);
        phils[i] = pthread_create(&phils[i], NULL, philosopher_stuff, phil);
    }
    sleep(20);

    // Join threads
    for(int i = 0; i < num; i++) {
        printf("join\n");
        pthread_join(phils[i], NULL);
    }
}
