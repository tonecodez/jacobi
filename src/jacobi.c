#include "barrier.h"

#define num_threads 1

/* Thread Function
 *
 * args  thread_info structure which contained all of the data relevant to the
 * the threads
 *
 * This function is responsible for calling the compute function, with each
 * thread having its own instance of the function.
 *
 */
void* thd_func(void* args);

/* Compute
 *
 * m1        matrix to retrieve the data from
 * m2        matrix where the results are inputted
 * row_from  current threads allocated portion to start calculating from
 * row_to    current threads allocated portion to end calculating to
 * thd_num   current threads thread number
 *
 * Recurses until the maxdiff for each of the threads is under the threshold.  
 * 
 */
void compute(double** m1, double** m2, int row_from, int row_to, int thd_num);

typedef struct thread_info {
   pthread_t thread_id;
   int thread_num;
   int row_from;
   int row_to;
   double** matrix;
   double** rslts;

} thread_info;

thread_info tinfo[num_threads];
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
barrier* diy_barrier;
double maxdiff[num_threads];
int t_exit = 0;

/* Main Entry Point
 *
 * No command line arguments are used.
 *
 * Reads in the data for the matrix and creates two 2-d arrays with a size of
 * 1024 by 1024 and inputs the data read into the arrays. Also creates the
 * p_threads. 
 *
 * Also responsible for writing the output data into an output file. Frees all
 * matrices and closes all files
 *
 */
int main(int argc, char* argv[]) {

   FILE* input;

   printf("Starting point for Jacobi with %d thread(s).\n", num_threads);
 
   if((input = fopen("input.mtx", "r")) == NULL) {
      fprintf(stderr, "Input file could not be found.\n");
      exit(1);
   }

   double** matrix = malloc(sizeof(*matrix)*1024);
   double** rslts = malloc(sizeof(*rslts)*1024);
   double curr_val;

   for(int i = 0; i < 1024; i++) {
      matrix[i] = malloc(sizeof(*matrix[i])*1024);
      rslts[i] = malloc(sizeof(*rslts[i])*1024);
   }

   for(int r = 0; r < 1024; r++) { 
      for(int c = 0; c < 1024; c++) {
         fscanf(input, "%lf", &curr_val);
         matrix[r][c] = curr_val;
         rslts[r][c] = curr_val;

      }
   }

   diy_barrier = barrier_new(num_threads);

   for(int j = 0; j < num_threads; j++) {

      tinfo[j].thread_num = j;
      tinfo[j].row_from = (((j) * (1024-2))/num_threads) + 1;
      tinfo[j].row_to = (((j+1) * (1024-2))/num_threads) + 1;
      tinfo[j].matrix = matrix;
      tinfo[j].rslts = rslts;

      maxdiff[j] = 1;

      if(pthread_create(&tinfo[j].thread_id, NULL, thd_func, &tinfo[j]) != 0) {
         fprintf(stderr, "pthread_create failure");
         exit(0);
      }

   }

   for(int k = 0; k < num_threads; k++) {
      if(pthread_join(tinfo[k].thread_id, NULL) != 0) {
         perror("pthread_join error");
         exit(0);
      }
   }


   FILE* output;
   if((output = fopen("myoutput.mtx", "w")) == NULL) {
      fprintf(stderr, "Output file could not be found.\n");
      exit(1);
   }

   for(int r = 0; r < 1024; r++)
     for(int c = 0; c < 1024; c++){
       fprintf(output, "%.10lf ", rslts[r][c]);
       if (c == 1023)
	      fprintf(output, "\n");
     }
       

   fclose(output);

   for(int i = 0; i < 1024; i++) {
      free(matrix[i]);
      free(rslts[i]);
   }

   free(matrix);
   free(rslts);

   fclose(input);
   return 0;
}

/* Thread Function
 *
 * args  thread_info structure which contained all of the data relevant to the
 * the threads
 *
 * This function is responsible for calling the compute function, with each
 * thread having its own instance of the function.
 *
 */
void* thd_func(void* args) {
   
   thread_info* curr_thread;
   curr_thread = (thread_info*) args;

   compute(curr_thread->matrix, curr_thread->rslts, 
           curr_thread->row_from, curr_thread->row_to, curr_thread->thread_num);
      
   pthread_exit(NULL);
}



/* Compute
 *
 * m1        matrix to retrieve the data from
 * m2        matrix where the results are inputted
 * row_from  current threads allocated portion to start calculating from
 * row_to    current threads allocated portion to end calculating to
 * thd_num   current threads thread number
 *
 * Recurses until the maxdiff for each of the threads is under the threshold.  
 * 
 */
void compute(double** m1, double** m2, int row_from, int row_to, int thd_num) {

   barrier_enter(diy_barrier);
   
   maxdiff[thd_num] = 0;
   t_exit = 0;
   
   barrier_enter(diy_barrier);

   for(int r = row_from; r < row_to; r++) {
      for(int c = 1; c < 1023; c++){
         
         m2[r][c] = (m1[r][c+1]+\
                     m1[r][c-1]+\
                     m1[r+1][c]+\
                     m1[r-1][c])/4;

         if (maxdiff[thd_num] < m2[r][c] - m1[r][c]) 
            maxdiff[thd_num] = m2[r][c] - m1[r][c];
      }
   }
   
   if(maxdiff[thd_num] > .00001)
      t_exit = 1;

   barrier_enter(diy_barrier);
   
   if(t_exit == 1)
      compute(m2, m1, row_from, row_to, thd_num);
}
