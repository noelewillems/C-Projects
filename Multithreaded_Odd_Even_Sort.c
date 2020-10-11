// Created by Noel Willems :(
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 

void swap(int a[], int i, int j) {
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}
void *compareEven(void* arg) {
    int* array = arg;
    for(int j = 0; j + 1 < 1000; j += 2) {
        if(array[j] > array[j+1]) {
            swap(array, j, j+1);
        }
    }
}

void *compareOdd(void* arg) {
    int* array = arg;
    for(int j = 1; j <= 1000; j += 2) {
        if(array[j] > array[j+2]) {
            swap(array, j, j+2);
        }
    }
}

int main(int argc, char *argv[]) { 
    pthread_t threads[2];
    int array[1000];
    for(int i = 0; i < 1000; i++) {
        array[i] = 1000+(rand()%9000);
    }
    printf("========Before Sort========\n");
    for(int i = 0; i < 100; i++) {
        printf("%d ", array[i]);
    }

    for(int i=0; i < 500; i++) {
        pthread_create(&threads[0], NULL, compareEven, array);
        pthread_create(&threads[1], NULL, compareOdd, array);
        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
    }

    printf("\n========After Sort========\n");
    for(int i = 0; i < 100; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}