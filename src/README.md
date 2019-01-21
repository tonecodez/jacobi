# jacobi
By Ricky Carroll and Tony Dinh
CSCI 347, Fall 2018
Western Washington University

To run the file, please compile with -pthread. Also, compile with both jacobi.c
and barrier.c.

The files required to run our program are input.mtx and myoutput.mtx.

An example of what your directory should look like is:

ls

barrier.c     input.mtx   jacobi.c   barrier.h
myoutput.mtx  README.md

\

To reproduce our results, on line 3 of the jacobi.c file, we defined a constant
that determined how many threads we wanted to use. We set the number of threads 
by default to be 1. When testing for each interval, we changed the number every 
time.

To calculate the time, we ran the program with
time ./jacobi
