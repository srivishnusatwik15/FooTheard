// computesum.c

#include "foothread.h"
#include <stdio.h>
#include <stdlib.h>

//global variables
int n;               
int *P;              // parent array of the tree
int *sums;           // array to store partial sums
int parent[100];
int num_children[100];
foothread_mutex_t *mutexes;  // array of mutexes for synch
foothread_mutex_t *io_mutex;  // mutex for I/O operations
foothread_barrier_t *barriers; // barrier for synch

void parse_file(const char* file_path) {
    FILE *file = fopen(file_path, "r");
    fscanf(file, "%d", &n); //read value of n
    // initialise parent and children arrays
    for (int i = 0; i < n; i++) {
        parent[i] = -1;
        num_children[i] = 0;
    }

    int child, parent_node;
    while (fscanf(file, "%d %d", &child, &parent_node) != EOF) {
        if(child!= parent_node){
        parent[child] = parent_node;
        num_children[parent_node]++;
        }
    }

    fclose(file);
}

// function to compute sum 
void computesum(int node) {
    // if node is a leaf node , read  input
    if (num_children[node] == 0) {
        int input;
        foothread_mutex_lock(io_mutex);
        printf("Lead node %d:: Enter a positive integer: ", node);
        scanf("%d", &input);
        foothread_mutex_unlock(io_mutex);

        
        foothread_mutex_lock(&mutexes[parent[node]]);
        sums[node] += input;//parents sum
        foothread_mutex_unlock(&mutexes[parent[node]]);
        // signal parent that sum has been updated
        foothread_barrier_wait(&barriers[parent[node]]);
    }
     else {
        // wait for children to finish computing sum
        foothread_barrier_wait(&barriers[node]);
        // update internal node sum
        int child_sum = 0;
        for (int i = 0; i < n; i++) {
            if (parent[i] == node) {
                child_sum += sums[i];
            }
        }
        printf("Internal node %d gets the partial sum %d from its children\n", node, child_sum);
        sums[node] = child_sum;
    }
    
    if(parent[node]!=-1){//if node not equal to root
        foothread_barrier_wait(&barriers[parent[node]]);//signal to its parent
    }
}



void* thread_func(void *arg) {
    int node = (intptr_t)arg;
    computesum(node);
    return NULL;
}

int main() {

    parse_file("tree.txt");


    // initialise synch resources
    barriers = (foothread_barrier_t *)malloc(n * sizeof(foothread_barrier_t));
    for (int i = 0; i < n; i++) {
        if (num_children[i] > 0)
           foothread_barrier_init(&barriers[i], num_children[i]+1);
    }
    mutexes = (foothread_mutex_t *)malloc(n * sizeof(foothread_mutex_t));
    io_mutex = (foothread_mutex_t *)malloc(sizeof(foothread_mutex_t));
    
    for (int i = 0; i < n; i++) {
        foothread_mutex_init(&mutexes[i]);
    }
    foothread_mutex_init(io_mutex);
    sums = (int *)malloc(n * sizeof(int));//memory allocation of sums array
    

    // Creating  threads for each node 
    foothread_t threads[n];
    for (int i = 0; i < n; i++) {
        foothread_attr_t attr = FOOTHREAD_ATTR_INITIALZER;
        foothread_attr_setjointype(&attr, FOOTHREAD_JOINABLE);
        foothread_create(&threads[i], &attr, thread_func, (void *)(intptr_t)i);
    }

    sleep(20);
    for (int i = 0; i < n; i++) {
        if(parent[i]==-1){
            printf("Sum of root (node %d) =  %d\n",i, sums[i]);
        }
    }

    free(P); //free memory allocations
    free(sums);
    free(mutexes);
    for (int i = 0; i < n; i++) {
        foothread_barrier_destroy(&barriers[i]);
    }
    return 0;
}