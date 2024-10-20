// File: thread-worker.c

// List all group member's name: Rohit Bellam, Enrico Aquino
// username of iLab: rsb204, eja97
// iLab Server: plastic
#include "thread-worker.h"

#define STACK_SIZE SIGSTKSZ

// Global counter for total context switches and averages
long tot_cntx_switches = 0;
double avg_turn_time = 0;
double avg_resp_time = 0;
double tot_resp_time = 0;
double tot_turn_time = 0;

// Initialize all your other variables her

int thread_counter = 0; // counts # of threads

Queue blockedQueue;
Queue terminatedQueue;

/* MLFQ levels */
#define MLFQ_LEVELS 4
Queue mlfq_queues[MLFQ_LEVELS]; // mlfq_queues stores the four levels of the queue

ucontext_t scheduler;        // Context for scheduler
ucontext_t benchmark;        // Context for benchmarks
ucontext_t context_main;     // Context for main thread creation call
struct itimerval sched_timer; // Timer
tcb* curThread = NULL;       // Currently running thread
int initialcall = 1;

/* How many quantums have elapsed in total */
int total_quantums_elapsed = 0;

/* create a new thread */
int worker_create(worker_t* thread, pthread_attr_t* attr,
                  void* (*function)(void*), void* arg) {
    // - create Thread Control Block (TCB)
    // - create and initialize the context of this worker thread
    // - allocate space of stack for this thread to run
    // after everything is set, push this thread into run queue and
    // - make it ready for the execution.


    // Create TCB, get context, make stack
    tcb* control_block = malloc(sizeof(tcb));
    if (getcontext(&control_block->context) < 0) {
        perror("getcontext");
        exit(1);
    }
    void* stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        perror("Failed to allocate stack");
        exit(1);
    }

    // Stack and context
    control_block->context.uc_link = &scheduler;
    control_block->context.uc_stack.ss_sp = stack;
    control_block->context.uc_stack.ss_size = STACK_SIZE;
    control_block->context.uc_stack.ss_flags = 0;
    control_block->stack = stack;

    // Other attributes
    control_block->thread_id = thread_counter;
    *thread = control_block->thread_id;
    thread_counter++;
    control_block->thread_status = ready;
    control_block->priority = 0; // Highest priority
    control_block->quantums_elapsed = 0;
    control_block->next = NULL;
    control_block->queued_time = clock();
    makecontext(&control_block->context, (void*)function, 1, arg);

    // Enqueue the new thread into the highest priority queue
    enqueue(&mlfq_queues[0], control_block);

    if (initialcall) {
        // Create context for scheduler and benchmark program
        scheduler_benchmark_create_context();
    }
    return 0;
}

#ifdef MLFQ
/* This function gets called only for MLFQ scheduling set the worker priority. */
int worker_setschedprio(worker_t thread, int prio) {

   // using the searchallqueues user function to retrieve the specific thread TCB
   tcb* f_thread = searchAllQueues(thread);

   // Set the priority value to your thread's TCB
   f_thread->priority = prio;

   return 0;

}
#endif

/* give CPU possession to other user-level worker threads voluntarily */
int worker_yield() {
    // - change worker thread's state from Running to Ready
    // - save context of this thread to its thread control block
    // - switch from thread context to scheduler context

    if (curThread != NULL) {
        curThread->thread_status = ready;
        tot_cntx_switches++;
        swapcontext(&curThread->context, &scheduler);
    }
    return 0;
}

/* terminate a thread */
void worker_exit(void* value_ptr) {
    disable_timer();
    curThread->thread_status = terminated;
    if (value_ptr) curThread->return_value = value_ptr;
    if (!curThread->end_time) curThread->end_time = clock();

    curThread->response_time = ((curThread->start_time - curThread->queued_time) * 1000) / CLOCKS_PER_SEC;
    curThread->turnaround_time = ((curThread->end_time - curThread->queued_time) * 1000) / CLOCKS_PER_SEC;

    tot_resp_time += curThread->response_time;
    tot_turn_time += curThread->turnaround_time;
}

/* Wait for thread termination */
int worker_join(worker_t thread, void** value_ptr) {
    // - wait for a specific thread to terminate
    // - de-allocate any dynamic memory created by the joining thread


    tcb* joining_thread = searchAllQueues(thread);
    if (joining_thread == NULL) {
        exit(1);
    }

    while (joining_thread->thread_status != terminated) {
        // Busy wait (could be optimized further)
    }

    if (value_ptr) *value_ptr = joining_thread->return_value; // Save return value
    if (joining_thread->stack) free(joining_thread->stack);   // Free thread memory
    free(joining_thread);

    return 0;
}

/* initialize the mutex lock */
int worker_mutex_init(worker_mutex_t* mutex, const pthread_mutexattr_t* mutexattr) {
    // - initialize data structures for this mutex


    if (mutex == NULL) return -1;
    if (mutex->initialized == 1) return -1;

    mutex->initialized = 1;
    mutex->locked = 0;
    mutex->lock_owner = NULL;

    return 0;
}

/* acquire the mutex lock */
int worker_mutex_lock(worker_mutex_t* mutex) {
    // - use the built-in test-and-set atomic function to test the mutex
    // - if the mutex is acquired successfully, enter the critical section
    // - if acquiring mutex fails, push current thread into block list and
    // context switch to the scheduler thread

    if (mutex == NULL) {
        return -1;
    }
    if (mutex->initialized == 0) {
        return -1;
    }

    // Attempt to acquire the lock
    while (__atomic_test_and_set(&mutex->locked, 1)) {
        curThread->thread_status = blocked;
        tot_cntx_switches++;
        swapcontext(&curThread->context, &scheduler);
    }

    // If lock is acquired, keep track of lock owner
    mutex->lock_owner = curThread;
    return 0;
}

/* release the mutex lock */
int worker_mutex_unlock(worker_mutex_t* mutex) {
    // - release mutex and make it available again.
    // - put threads in block list to run queue
    // so that they could compete for mutex later.


    if (mutex == NULL) {
        return -1;
    }
    if (mutex->initialized == 0 || mutex->locked == 0) {
        return -1;
    }
    if (mutex->lock_owner != curThread) {
        return -1;
    }

    // Remove thread from blocked queue and add to highest priority queue
    blockedDequeue();

    // Release the lock
    mutex->locked = 0;
    mutex->lock_owner = NULL;

    return 0;
}

/* destroy the mutex */
int worker_mutex_destroy(worker_mutex_t* mutex) {
    // - de-allocate dynamic memory created in worker_mutex_init

    // Check for valid mutex and lock status
    if (mutex == NULL) {
        return -1;
    }
    if (mutex->initialized == 0 || mutex->locked == 1) {
        return -1;
    }

    mutex->initialized = 0;
    mutex->locked = 0;
    mutex->lock_owner = NULL;

    return 0;
}

/* scheduler */
static void schedule() {
    // - every time a timer interrupt occurs, your worker thread library
    // should be context switched from a thread context to this
    // schedule() function

    // - invoke scheduling algorithms according to the policy (PSJF or MLFQ)


    while (!areQueuesEmpty()) {
        disable_timer();
        // - schedule policy
#ifndef MLFQ
        sched_psjf();
#else
        sched_mlfq();
#endif
        if (!curThread->start_time) curThread->start_time = clock();
        curThread->thread_status = running;
        enable_timer();
        if (curThread != NULL) {
            tot_cntx_switches++;
            swapcontext(&scheduler, &curThread->context);
            curThread->quantums_elapsed++;
        }
        if (curThread->thread_status != terminated && curThread->thread_status != blocked) {
            curThread->thread_status = ready;
#ifndef MLFQ
            enqueue(&mlfq_queues[0], curThread); // For PSJF, we can use priority 0 queue
#else
            enqueueMLFQ(curThread);
#endif
        } else if (curThread->thread_status == blocked) {
            enqueue(&blockedQueue, curThread);
        } else if (curThread->thread_status == terminated) {
            enqueue(&terminatedQueue, curThread);
        }
    }
}

/* Pre-emptive Shortest Job First (POLICY_PSJF) scheduling algorithm */
static void sched_psjf() {
    // - your own implementation of PSJF

    curThread = dequeuePSJF(&mlfq_queues[0]); // For PSJF, use priority 0 queue
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
    // - your own implementation of MLFQ

    dequeueMLFQ();
}

// DO NOT MODIFY THIS FUNCTION
/* Function to print global statistics. Do not modify this function.*/
void print_app_stats(void) {
    avg_resp_time = tot_resp_time / thread_counter;
    avg_turn_time = tot_turn_time / thread_counter;
    fprintf(stderr, "Total context switches %ld \n", tot_cntx_switches);
    fprintf(stderr, "Average turnaround time %lf \n", avg_turn_time);
    fprintf(stderr, "Average response time  %lf \n", avg_resp_time);
}

// Feel free to add any other functions you need

void enqueue(Queue* queue, tcb* thread) {
    if (queue->tail == NULL) {
        queue->head = queue->tail = thread;
    } else {
        queue->tail->next = thread;
        queue->tail = thread;
    }
    thread->next = NULL;
}

tcb* dequeue(Queue* queue) {
    tcb* thread = queue->head;
    if (thread == NULL) return NULL;
    queue->head = thread->next;
    if (queue->head == NULL) queue->tail = NULL;
    thread->next = NULL;
    return thread;
}

void enqueueMLFQ(tcb* thread) {
    if (thread->priority >= MLFQ_LEVELS) {
        thread->priority = MLFQ_LEVELS - 1;
    }
    enqueue(&mlfq_queues[thread->priority], thread);
}

tcb* dequeuePSJF(Queue* queue) {
    if (queue->head == NULL) return NULL;
    tcb* target = queue->head; // Head of queue
    tcb* cur = queue->head;
    tcb* prev = NULL;
    tcb* prevTarget = NULL;

    // Find thread with lowest quantums_elapsed
    while (cur != NULL) {
        if (cur->quantums_elapsed < target->quantums_elapsed) {
            target = cur;
            prevTarget = prev;
        }
        prev = cur;
        cur = cur->next;
    }

    // Remove target from queue
    if (prevTarget == NULL) {
        // Target is at head
        queue->head = target->next;
        if (queue->head == NULL) queue->tail = NULL;
    } else {
        prevTarget->next = target->next;
        if (target == queue->tail) queue->tail = prevTarget;
    }
    target->next = NULL;
    return target;
}

void dequeueMLFQ() {
    for (int i = 0; i < MLFQ_LEVELS; i++) {
        if ((curThread = dequeue(&mlfq_queues[i])) != NULL) {
            return;
        }
    }
}

void blockedDequeue() {
    tcb* temp = dequeue(&blockedQueue);
    if (temp != NULL) {
        temp->thread_status = ready;
        temp->priority = 0; // Reset priority when unblocked
        enqueue(&mlfq_queues[0], temp);
    }
}

void resetMLFQ() {
    // Set all thread priorities to 0 and enqueue to top-most queue (mlfq_queues[0])
    if (curThread != NULL) curThread->priority = 0;

    tcb* temp;

    // Reset priorities in blocked queue
    temp = blockedQueue.head;
    while (temp != NULL) {
        temp->priority = 0;
        temp = temp->next;
    }

    // Reset priorities and move threads from lower queues to the top queue
    for (int i = 1; i < MLFQ_LEVELS; i++) {
        while ((temp = dequeue(&mlfq_queues[i])) != NULL) {
            temp->priority = 0;
            enqueue(&mlfq_queues[0], temp);
        }
    }
}

static tcb* search(worker_t thread, Queue* queue) {
    tcb* temp = queue->head;
    while (temp != NULL) {
        if (temp->thread_id == thread) return temp;
        temp = temp->next;
    }
    return NULL;
}

static tcb* searchAllQueues(worker_t thread) {
    tcb* joining_thread = NULL;

    // Search MLFQ queues
    for (int i = 0; i < MLFQ_LEVELS; i++) {
        if ((joining_thread = search(thread, &mlfq_queues[i])) != NULL) {
            return joining_thread;
        }
    }

    // Search blocked and terminated queues
    if ((joining_thread = search(thread, &blockedQueue)) != NULL) {
        return joining_thread;
    }
    if ((joining_thread = search(thread, &terminatedQueue)) != NULL) {
        return joining_thread;
    }

    return NULL;
}

int areQueuesEmpty() {
#ifndef MLFQ
    return (mlfq_queues[0].head == NULL);
#else
    for (int i = 0; i < MLFQ_LEVELS; i++) {
        if (mlfq_queues[i].head != NULL) return 0;
    }
    return 1;
#endif
}

void printQueue(Queue* queue) {
    tcb* temp = queue->head;
    while (temp != NULL) {
        printf("thread %d, ", temp->thread_id);
        temp = temp->next;
    }
    printf("\n");
}

void toString(tcb* thread) {
    printf("Thread id: %d\nStatus: %d\n\n", thread->thread_id, thread->thread_status);
}

static void signal_handler(int signum) {
    if (curThread != NULL) {
#ifdef MLFQ
        if (curThread->priority < MLFQ_LEVELS - 1) curThread->priority++;
        if (++total_quantums_elapsed >= S) {
            total_quantums_elapsed = 0;
            resetMLFQ();
        }
#endif
        tot_cntx_switches++;
        swapcontext(&curThread->context, &scheduler);
    }
}

static void enable_timer() {
    sched_timer.it_interval.tv_usec = TIME_US;
    sched_timer.it_interval.tv_sec = TIME_S;

    sched_timer.it_value.tv_usec = TIME_US;
    sched_timer.it_value.tv_sec = TIME_S;
    setitimer(ITIMER_PROF, &sched_timer, NULL);
}

static void disable_timer() {
    sched_timer.it_interval.tv_usec = 0;
    sched_timer.it_interval.tv_sec = 0;

    sched_timer.it_value.tv_usec = 0;
    sched_timer.it_value.tv_sec = 0;
    setitimer(ITIMER_PROF, &sched_timer, NULL);
}

void setup_timer() {
    // Use sigaction to register signal handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &signal_handler;
    sigaction(SIGPROF, &sa, NULL);

    // Set up the timer
    sched_timer.it_interval.tv_usec = TIME_US;
    sched_timer.it_interval.tv_sec = TIME_S;

    sched_timer.it_value.tv_usec = TIME_US;
    sched_timer.it_value.tv_sec = TIME_S;

    // Start the timer
    setitimer(ITIMER_PROF, &sched_timer, NULL);
}

int scheduler_benchmark_create_context() {
    initialcall = 0;
    getcontext(&scheduler);
    void* stack = malloc(SIGSTKSZ);
    scheduler.uc_link = NULL;
    scheduler.uc_stack.ss_sp = stack;
    scheduler.uc_stack.ss_size = STACK_SIZE;
    scheduler.uc_stack.ss_flags = 0;
    
    makecontext(&scheduler, (void*)&schedule, 0, NULL);
    setup_timer();

    getcontext(&context_main);

    tcb* mainTCB = malloc(sizeof(tcb));

    // Other attributes
    mainTCB->thread_id = thread_counter;
    thread_counter++;
    mainTCB->thread_status = ready;
    mainTCB->priority = 0;
    mainTCB->quantums_elapsed = 0;
    mainTCB->next = NULL;
    mainTCB->queued_time = clock();
    enqueue(&mlfq_queues[0], mainTCB);
    tot_cntx_switches++;
    swapcontext(&mainTCB->context, &scheduler);
    return 0;
}