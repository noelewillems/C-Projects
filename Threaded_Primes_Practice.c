/*
Modified code from John Hunt.
Demonstrates concurrency "bugs."
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define NANO_PER_SEC 1000000000.0
#define FALSE 0
#define TRUE !(FALSE)

int p_cnt = 1; /* number of primes found starting with 2 */

/* passed into prime_finder threads */
typedef struct prime_finder_vars {
   long from;
   long to;
} PrimeFinderVars;

int is_prime(long num) {
/* return whether num is prime */
   int limit = round(sqrt(num)); 
   for(long i=2; i<=limit; i++) {
      if(num % i == 0)
         return FALSE;
   }
   return TRUE;
}

void *prime_finder(void *pf) {
   // Initialize pf_vars pointer of type PrimeFindersVars
   PrimeFinderVars *pf_vars= (PrimeFinderVars *)pf;
   // Assign number "next_cand" to above's "from"
   long next_cand = pf_vars->from;
   // While we haven't reached the "to" of pf_vars (aka the max)
   while(next_cand < pf_vars->to) {
      if(is_prime(next_cand)) {
         p_cnt++;
      }
      next_cand += 2; 
   }
}


int main(void) {
   struct timespec start;
   struct timespec end;
   double start_sec, end_sec, elapsed_sec;
   clock_gettime(CLOCK_REALTIME, &start);

   /* calc primes */
   int rc;
   
   // FIRST QUARTER OF 8 MIL: 3 to 2,000,000
   // Creating struct of type PrimeFindersVars
   PrimeFinderVars thr1_vars; 
   thr1_vars.from = 3;
   thr1_vars.to = 2000000;
   // Create thread called thr1
   pthread_t thr1;
   // Setting int rc to created thread called "thr1", sending it to method prime_finder, and sending arg of thr1_vars (sets from and to)
   rc = pthread_create(&thr1,NULL,prime_finder,&thr1_vars);

   // SECOND QUARTER OF 8 MIL: 2,000,0001 - 4,000,000
   // Creating variable thr2_vars of type PrimeFindersVars
   PrimeFinderVars thr2_vars; 
   thr2_vars.from = 2000001;
   thr2_vars.to = 4000000;
   pthread_t thr2;
   // Reset int rc to created thread called "thr2", sending it to method prime_finder, and sending arg of thr2_vars (sets from and to)
   rc = pthread_create(&thr2,NULL,prime_finder,&thr2_vars);

   // THIRD QUARTER OF 8 MIL: 4,000,001 - 6,000,000
   // Creating variable thr3_vars of type PrimeFindersVars
   PrimeFinderVars thr3_vars; 
   thr3_vars.from = 4000001;
   thr3_vars.to = 6000000;
   pthread_t thr3;
   // Reset int rc to created thread called "thr3", sending it to method prime_finder, and sending arg of thr3_vars (sets from and to)
   rc = pthread_create(&thr3,NULL,prime_finder,&thr3_vars);

   // FOURTH QUARTER OF 8 MIL: 6,000,001 - 8,000,000
   // Creating variable thr4_vars of type PrimeFindersVars
   PrimeFinderVars thr4_vars; 
   thr4_vars.from = 6000001;
   thr4_vars.to = 8000000;
   pthread_t thr4;
   // Reset int rc to created thread called "thr4", sending it to method prime_finder, and sending arg of thr4_vars (sets from and to)
   rc = pthread_create(&thr4,NULL,prime_finder,&thr4_vars);

   pthread_join(thr1, NULL);
   pthread_join(thr2, NULL);
   pthread_join(thr3, NULL);
   pthread_join(thr4, NULL);

   clock_gettime(CLOCK_REALTIME, &end);

   start_sec = start.tv_sec + start.tv_nsec/NANO_PER_SEC;
   end_sec = end.tv_sec + end.tv_nsec/NANO_PER_SEC;
   elapsed_sec = end_sec - start_sec;
   printf("elapsed time %2.5f seconds\n", elapsed_sec);
   printf("number of primes found %d\n",p_cnt);
}