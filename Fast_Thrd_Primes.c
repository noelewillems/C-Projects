// Noel Willems
// Compile example: gcc fast_threaded_primes.c -lm -pthread -o ftp


// I didn't run into any race condition problems because there were no global variables that I was changing the value of.
// I decided to go ahead and put "count" in the struct of the prime_finder_vars so that it would be local and part of the
// "object." I was a little hesitant to try and use locks.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "Array.h"
#include <pthread.h>

#define NANO_PER_SEC 1000000000.0
#define FALSE 0
#define TRUE !(FALSE)

// Array of primes
Array primes;
// Struct for prime finder variables
typedef struct prime_finder_vars {
   long from;
   long to;
   long cnt;
} PrimeFinderVars;
// Checks if num is prime and adds it to the array
int is_prime(long num) {
   long limit = round(sqrt(num)); 
   long val; 
   long i = 1; /*start at 1 since 2 is first and num is odd*/
   while((i<primes.used) &&
      ((val = primes.array[i]) <= limit)) {
      if(num % val == 0)
         return FALSE;
      i++;
   }
   return TRUE;
}

void *prime_finder(void *pf) {
   // Initialize pf_vars pointer of type PrimeFindersVars
   PrimeFinderVars *pf_vars= (PrimeFinderVars *)pf;
   long cnt = pf_vars->cnt;
   // Assign number "next_cand" to above's "from"
   long next_cand = pf_vars->from;
   // While we haven't reached the "to" of pf_vars (aka the max)
   while(next_cand < pf_vars->to) {
      if(is_prime(next_cand)) {
         pf_vars->cnt = pf_vars->cnt + 1;
      }
      next_cand += 2; 
   }
}

int main(void) {
   // Time stuff
   struct timespec start;
   struct timespec end;
   double start_sec, end_sec, elapsed_sec;
   long limit = round(sqrt(8000000)); 
   clock_gettime(CLOCK_REALTIME, &start);

   // SMALL PRIMES
   // Initializes array beginning with 5
   initArray(&primes,5);
   // Inserts 2 because it won't be included in the before one
   insertArray(&primes, 2);
   // Next candidate starts at 3 (goes to every odd number)
   long next_cand = 3; 
   while(next_cand < limit) {
      if(is_prime(next_cand)) {
         insertArray(&primes, next_cand);
      }
      next_cand += 2;
   }

   // Thread 1 - end of small primes --> half of 8 mil
   PrimeFinderVars thr1_vars; 
   thr1_vars.cnt = 0;
   thr1_vars.from = 2829;
   thr1_vars.to = 4000000;
   pthread_t thr1;
   int t1;
   t1 = pthread_create(&thr1,NULL,prime_finder,&thr1_vars);
   // Thread 2 - half of 8 mil --> 8 mil
   PrimeFinderVars thr2_vars; 
   thr2_vars.cnt = 0;
   thr2_vars.from = 4000001;
   thr2_vars.to = 8000001;
   pthread_t thr2;
   int t2;
   t2 = pthread_create(&thr2,NULL,prime_finder,&thr2_vars);

   // Join threads
   pthread_join(thr1, NULL);
   pthread_join(thr2, NULL);

   int total = (int)primes.used + thr1_vars.cnt + thr2_vars.cnt;
   clock_gettime(CLOCK_REALTIME, &end);
   start_sec = start.tv_sec + start.tv_nsec/NANO_PER_SEC;
   end_sec = end.tv_sec + end.tv_nsec/NANO_PER_SEC;
   elapsed_sec = end_sec - start_sec;
   printf("elapsed time %2.5f seconds\n", elapsed_sec);
   printf("number of primes found %d\n", total);

   // Freeing the Array of primes
   freeArray(&primes);
}