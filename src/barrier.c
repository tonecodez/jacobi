#include "barrier.h"

/* Barrier New
 *
 * count  current number of threads
 *
 * Creates a new barrier.
 *
 */
barrier* barrier_new(int count) {

   barrier* b = malloc(sizeof(barrier));

   if(b != NULL) {

      pthread_mutex_init(&(b->m), NULL);
      pthread_cond_init(&(b->all_here), NULL);
      pthread_cond_init(&(b->all_gone), NULL);
      b->total = count;
      b->here = 0;
      b->leaving = 0;
   }

   return b;

}

/* Barrier Enter
 *
 * b  current barrier
 *
 * Threads enter the given barrier and only leave once all threads have entered.
 *
 */
void barrier_enter(barrier* b) {

   pthread_mutex_lock(&(b->m));

   while(b->leaving) 
      pthread_cond_wait(&(b->all_gone), &(b->m));
   
   pthread_cond_signal(&(b->all_gone));
   b->here++;

   while(b->here != b->total && b->leaving == 0)
      pthread_cond_wait(&(b->all_here), &(b->m));
   
   b->leaving = 1;
   pthread_cond_signal(&(b->all_here));
   b->here--;

   if(b->here == 0) {
      b->leaving = 0;
      pthread_cond_signal(&(b->all_gone));
   }

   pthread_mutex_unlock(&(b->m));
}
