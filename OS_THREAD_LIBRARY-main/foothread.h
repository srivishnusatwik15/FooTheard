#ifndef FOOTHREAD_H
#define FOOTHREAD_H
#define _GNU_SOURCE
#include <semaphore.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>



#define FOOTHREAD_THREADS_MAX 100  //max number of threads
#define FOOTHREAD_DEFAULT_STACK_SIZE (2 * 1024 * 1024) //defualt stach_size is 2MB


#define FOOTHREAD_JOINABLE 0
#define FOOTHREAD_DETACHED 1

#define FOOTHREAD_ATTR_INITIALZER {FOOTHREAD_DETACHED,FOOTHREAD_DEFAULT_STACK_SIZE}




// thread struct
typedef struct {
    pid_t leader_pid;             // PID of the leader thread
    pid_t tid;                    // TID of the follower thread
    void *(*thread_func)(void *);
    void *arg;
    int state;             // Detached or joinable state
    int joined;                   // to check if thread has been joined
} foothread_t;

// thread attr struct
typedef struct {
    int join_type;                // joinable or detached
    int stack_size;               // stack size
} foothread_attr_t;

// mutex struct
typedef struct {
    sem_t mutex;
    int locked; 
} foothread_mutex_t;

// barrier struct
typedef struct {
    sem_t barrier_sem1;
    sem_t barrier_sem2;
    int count;
    int num_threads;
} foothread_barrier_t;

typedef void *(*ThreadFunc)(void *);

// functions
int foothread_create(foothread_t *thread, foothread_attr_t *attr, ThreadFunc func, void *arg);
void foothread_attr_setjointype ( foothread_attr_t * , int ) ;
void foothread_attr_setstacksize ( foothread_attr_t * , int ) ;
void foothread_exit();
void foothread_mutex_init(foothread_mutex_t *mutex);
void foothread_mutex_lock(foothread_mutex_t *mutex);
void foothread_mutex_unlock(foothread_mutex_t *mutex);
void foothread_mutex_destroy(foothread_mutex_t *mutex);
void foothread_barrier_init(foothread_barrier_t *barrier, int num_threads);
void foothread_barrier_wait(foothread_barrier_t *barrier);
void foothread_barrier_destroy(foothread_barrier_t *barrier);

#endif