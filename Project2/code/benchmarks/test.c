// File: test.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "../thread-worker.h"

// Initialize mutex and the shared counter
worker_mutex_t mutex;
int shared_counter = 0;

/* Function that will be executed by the worker threads */
void *worker_function(void *arg)
{
    int thread_num = *((int *)arg);
    printf("Thread %d: Starting work\n", thread_num);

    // Simulate some work
    for (int i = 0; i < 5; i++)
    {
        printf("Thread %d: Working... (%d)\n", thread_num, i);
        usleep(500000); // Sleep for 0.5 seconds
        worker_yield(); // Yield to allow other threads to run
    }

    printf("Thread %d: Finished work\n", thread_num);
    worker_exit(NULL);
    return NULL;
}

void *mutex_test_function(void *arg)
{
    int thread_num = *((int *)arg);

    for (int i = 0; i < 5; i++)
    {
        worker_mutex_lock(&mutex);

        int temp = shared_counter;
        printf("Thread %d: Read shared_counter = %d\n", thread_num, temp);
        temp++;
        usleep(100000); // Sleep for 0.1 seconds to simulate work
        shared_counter = temp;
        printf("Thread %d: Updated shared_counter to %d\n", thread_num, shared_counter);

        worker_mutex_unlock(&mutex);
        worker_yield(); // Yield to allow other threads to run
    }

    worker_exit(NULL);
    return NULL;
}

int main(int argc, char **argv)
{

    #ifdef MLFQ
        //We use it only for MLFQ
        int priority = 0;
    #endif

    if (argc > 2 || argc == 1) {
        printf("Error: Invalid number of arguments. \n Usage: ./test NUNBER_OF_THREADS \n");
        return;
    }

    int num_threads = atoi(argv[1]);

    if (num_threads > 25) {
        printf("Error: Max number of threads reached. Please use less than 25 threads! \n");
        return;
    }

    pthread_t threads[num_threads];
    int thread_args[num_threads];

    // Create threads
    for (int i = 0; i < num_threads; i++)
    {
        thread_args[i] = i + 1;
        if (pthread_create(&threads[i], NULL, worker_function, &thread_args[i]) != 0)
        {
            perror("pthread_create");
            exit(1);
        }
        #ifdef MLFQ
                priority = i % NUMPRIO;
                pthread_setschedprio(threads[i], priority);
        #endif
    }

    // Initialize mutex
    worker_mutex_init(&mutex, NULL);

    // Join threads
    for (int i = 0; i < num_threads; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("pthread_join");
            exit(1);
        }
    }

    // Create threads for mutex test
    for (int i = 0; i < num_threads; i++)
    {
        thread_args[i] = i + 1;
        if (pthread_create(&threads[i], NULL, mutex_test_function, &thread_args[i]) != 0)
        {
            perror("pthread_create");
            exit(1);
        }
    }

    // Join threads
    for (int i = 0; i < num_threads; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("pthread_join");
            exit(1);
        }
    }

    // Destroy mutex
    worker_mutex_destroy(&mutex);

    printf("\nFinal value of shared_counter: %d\n", shared_counter);

    return 0;
}
