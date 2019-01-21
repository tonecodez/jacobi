#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

typedef struct barrier {
   pthread_mutex_t m;
   pthread_cond_t all_here;
   pthread_cond_t all_gone;
   int total;
   int here;
   int leaving;
} barrier;

/* Barrier New
 *
 * count  current number of threads
 *
 * Creates a new barrier.
 *
 */
barrier* barrier_new(int count);

/* Barrier Enter
 *
 * b  current barrier
 *
 * Threads enter the given barrier and only leave once all threads have entered.
 *
 */
void barrier_enter(barrier* b);
