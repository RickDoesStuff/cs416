// File:	thread-worker.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "thread-worker.h"

#define STACK_SIZE 1024 * 8 // 8,192 bytes for stack, default stack size
#define QUANTUM 10 * 1000 // For the Time Quantum

#define MAX_LEVELS 4 // can be adjusted for the MLFQ
queue_t mlfq[MAX_LEVELS];
// this is the variable that will hold all of the four separate queues

//Global counter for total context switches and 
//average turn around and response time
long tot_cntx_switches=0;
double avg_turn_time=0;
double avg_resp_time=0;

// INITAILIZE ALL YOUR OTHER VARIABLES HERE

int init_sched_finish = 0;
static worker_t next_thread_id = 0;
ucontext_t sched_context, main_context;
queue_t main_queue;

/* create a new thread */
int worker_create(worker_t * thread, pthread_attr_t * attr, 
					void *(*function)(void*), void * arg) {

	// - create Thread Control Block (TCB)

	// allocate the size of the new thread
	tcb *new_thread = (tcb *)malloc(sizeof(tcb));

	// if not enough memory, throw and error
	if (new_thread == NULL) {
		perror("Unable to allocate memory for TCB");
		exit(1);
	}

	// get the next thread id
	new_thread->tid = next_thread_id++;
	printf("thread id is %i\n",new_thread->tid);

	// - allocate space of stack for this thread to run
	new_thread->stack = malloc(STACK_SIZE);
	// if not enough memory, throw an error
	if (new_thread->stack == NULL) {
		perror("Unable to allocate memory for stack");
		exit(1);
	}


	// - create and initialize the context of this worker thread

	// get the current context and make sure it was gotten without error
	if (getcontext(&(new_thread->ctx)) < 0) {
		perror("getcontext");
		exit(1);
	}

	/* Setup context that we are going to use */
	new_thread->ctx.uc_link = NULL;
	new_thread->ctx.uc_stack.ss_sp = new_thread->stack;
	new_thread->ctx.uc_stack.ss_size = STACK_SIZE;
	new_thread->ctx.uc_stack.ss_flags = 0;
	
	printf("about to call make context\n");
	// setup the context to start running at the given function with given 1 arg *void type
	makecontext(&(new_thread->ctx), function, 1, arg);
	printf("successfully modified context\n");

	// after everything is set, push this thread into run queue and 
	// - make it ready for the execution.
	// set READY status
	new_thread->status = READY;
	// set Default priority
	new_thread->priority = DEFAULT_PRIO;

	// returns the tid to the caller
	*thread = new_thread->tid;

	return 0; // completed
};


#ifdef MLFQ
/* This function gets called only for MLFQ scheduling set the worker priority. */
int worker_setschedprio(worker_t thread, int prio) {


   // Set the priority value to your thread's TCB
   // YOUR CODE HERE

   return 0;	

}
#endif



/* give CPU possession to other user-level worker threads voluntarily */
int worker_yield() {
	
	// - change worker thread's state from Running to Ready
	// - save context of this thread to its thread control block
	// - switch from thread context to scheduler context

	// YOUR CODE HERE
	
	return 0;
};

/* terminate a thread */
void worker_exit(void *value_ptr) {
	// - de-allocate any dynamic memory created when starting this thread

	// YOUR CODE HERE
};


/* Wait for thread termination */
int worker_join(worker_t thread, void **value_ptr) {
	
	// - wait for a specific thread to terminate
	// - de-allocate any dynamic memory created by the joining thread
  
	// YOUR CODE HERE
	return 0;
};

/* initialize the mutex lock */
int worker_mutex_init(worker_mutex_t *mutex, 
                          const pthread_mutexattr_t *mutexattr) {
	//- initialize data structures for this mutex

	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int worker_mutex_lock(worker_mutex_t *mutex) {

        // - use the built-in test-and-set atomic function to test the mutex
        // - if the mutex is acquired successfully, enter the critical section
        // - if acquiring mutex fails, push current thread into block list and
        // context switch to the scheduler thread

        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int worker_mutex_unlock(worker_mutex_t *mutex) {
	// - release mutex and make it available again. 
	// - put threads in block list to run queue 
	// so that they could compete for mutex later.

	// YOUR CODE HERE
	return 0;
};


/* destroy the mutex */
int worker_mutex_destroy(worker_mutex_t *mutex) {
	// - de-allocate dynamic memory created in worker_mutex_init

	return 0;
};

/* scheduler */
static void schedule() {
	// - every time a timer interrupt occurs, your worker thread library 
	// should be contexted switched from a thread context to this 
	// schedule() function

	// - invoke scheduling algorithms according to the policy (PSJF or MLFQ)

	// if (sched == PSJF)
	//		sched_psjf();
	// else if (sched == MLFQ)
	// 		sched_mlfq();

	// YOUR CODE HERE

// - schedule policy
#ifndef MLFQ
	// Choose PSJF
#else 
	// Choose MLFQ
#endif

}

/* Pre-emptive Shortest Job First (POLICY_PSJF) scheduling algorithm */
static void sched_psjf() {
	// - your own implementation of PSJF
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}


/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// - your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

//DO NOT MODIFY THIS FUNCTION
/* Function to print global statistics. Do not modify this function.*/
void print_app_stats(void) {

       fprintf(stderr, "Total context switches %ld \n", tot_cntx_switches);
       fprintf(stderr, "Average turnaround time %lf \n", avg_turn_time);
       fprintf(stderr, "Average response time  %lf \n", avg_resp_time);
}


// Feel free to add any other functions you need

// YOUR CODE HERE

/*
Have functions such as prepare_scheduler/initialize scheduler, schedule mlfq, schedule sjf, enqueue, dequeue,
create start worker context, start worker, mlfq enqueue, mlfq dequeue
*/