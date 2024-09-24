/*
* Add NetID and names of all project partners
eja97 Enrico Aquino
rsb204 Rohit Bellam
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_t t1, t2, t3, t4;
pthread_mutex_t mutex;
int x = 0;
int loop = 10000;

/* Counter Incrementer function:
 * This is the function that each thread will run which
 * increments the shared counter x by LOOP times.
 *
 * Your job is to implement the incrementing of x
 * such that update to the counter is sychronized across threads.
 *
 */
void *add_counter(void *arg) {

    int i;

    /* Add thread synchronizaiton logic in this function */	

    for(i = 0; i < loop; i++){
    pthread_mutex_lock(&mutex);
	x = x + 1;
    pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


/* Main function:
 * This is the main function that will run.
 *
 * Your job is to create four threads and have them
 * run the add_counter function().
 */
int main(int argc, char *argv[]) {

    if(argc != 2){
        printf("Bad Usage: Must pass in a integer\n");
        exit(1);
    }

    loop = atoi(argv[1]);

    printf("Going to run four threads to increment x up to %d\n", 4 * loop);

    /* Implement Code Here */

    // create the four pthreads
    pthread_create(&t1, NULL, add_counter, NULL);
    pthread_create(&t2, NULL, add_counter, NULL);
    pthread_create(&t3, NULL, add_counter, NULL);
    pthread_create(&t4, NULL, add_counter, NULL);

    // join all of the threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    /* Destroy the mutex */
    pthread_mutex_destroy(&mutex);

    /* Make sure to join the threads */

    printf("The final value of x is %d\n", x);

    return 0;
}
