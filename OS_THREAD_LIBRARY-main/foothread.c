#include "foothread.h"

int thread_count = 0;//number of threads created

// function to initialise a mutex
void foothread_mutex_init(foothread_mutex_t *mutex) {
    sem_init(&(mutex->mutex), 1, 1);  // initilaise semaphore as a mutex
    mutex->locked = 0; // initialize the locked state to unlocked
}

// function to lock the mutex
void foothread_mutex_lock(foothread_mutex_t *mutex) {
    sem_wait(&(mutex->mutex));  // Wait until semaphore value becomes positive
    mutex->locked = 1; // chnage the locked state
}

// function to unlock the mutex
void foothread_mutex_unlock(foothread_mutex_t *mutex) {
    if (mutex->locked) { // Check if the mutex is locked
        sem_post(&(mutex->mutex));  // Increment semaphore value
        mutex->locked = 0; // Update the locked state
    } else {
        // Error: Attempting to unlock an unlocked mutex
        fprintf(stderr, " cannot unlock  an unlocked mutex\n");
        exit(EXIT_FAILURE); 
    }
}

// function to destroy the mutex
void foothread_mutex_destroy(foothread_mutex_t *mutex) {
    sem_destroy(&(mutex->mutex)); 
}

// function to initialize the barrier
void foothread_barrier_init(foothread_barrier_t *barrier, int num_threads) {
     
    barrier->count = 0;
    barrier->num_threads = num_threads;
    sem_init(&(barrier->barrier_sem1), 0, 0);  //initilaise barrier sem 1
    sem_init(&(barrier->barrier_sem2), 0, 1);  // initilaise barrier sem 2
}

// function to wait at a barrier
void foothread_barrier_wait(foothread_barrier_t *barrier) {
    sem_wait(&(barrier->barrier_sem2));  // only one thread can access count 
    barrier->count++;
    if (barrier->count == barrier->num_threads) {//last thread
        
        sem_post(&(barrier->barrier_sem1));  // release waiting threads
        sem_wait(&(barrier->barrier_sem2));  // block until all threads leave
        barrier->count = 0;  // reset count 
    } else {
        sem_post(&(barrier->barrier_sem2));  // release count access
        sem_wait(&(barrier->barrier_sem1));  // block until all threads arrive
    }
    sem_post(&(barrier->barrier_sem2));  // release waiting threads
}

// function to destroy a barrier
void foothread_barrier_destroy(foothread_barrier_t *barrier) {
    sem_destroy(&(barrier->barrier_sem1));
    sem_destroy(&(barrier->barrier_sem2));
}

// function to set join type attribute
void foothread_attr_setjointype(foothread_attr_t *attr, int join_type) {
    attr->join_type = join_type;
}

// function to set stack size attribute
void foothread_attr_setstacksize(foothread_attr_t *attr, int stack_size) {
    attr->stack_size = stack_size;
}




// function to create foothread
int foothread_create(foothread_t *thread, foothread_attr_t *attr, ThreadFunc func, void *arg) {
    
    thread->state = attr->join_type;//set state to join type
    thread->thread_func = func;//set thread fucntion
    thread->arg = arg;//set arguments

    
    int clone_flags = CLONE_VM | CLONE_SIGHAND | CLONE_THREAD;
    void *stack = malloc(attr->stack_size);
    thread->tid = clone(thread->thread_func, (char *)stack + attr->stack_size, clone_flags, thread->arg);
    
    thread->tid = gettid();//set tid of follower
    thread->leader_pid = getppid();

    return 0;
}