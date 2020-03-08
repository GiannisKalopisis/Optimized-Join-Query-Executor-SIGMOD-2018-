#ifndef JOB_SCHEDULER_H
#define JOB_SCHEDULER_H

#include <pthread.h>
#include <stdint.h>

#include "../include/relations.h"
#include "../include/hash_functions.h"

#define NUM_THREADS 8
// Suppose H1 makes more buckets
#define JOB_QUEUE_SIZE H1BUCKETS

typedef struct Job {
    void (*jobRoutine)(void *);
    void *arguments;
} Job;

typedef struct JobScheduler {
    // Threads
    pthread_t *threads;
    int numThreads;

    // Circular Job Queue
    Job *queue;
    int numJobs;
    int queueFront;
    int queueRear;
    int queueCount;

    // Mutexes and Conditions
    pthread_mutex_t queuelock;
    pthread_mutex_t joblock;
    pthread_cond_t cond_nonempty;
    pthread_cond_t cond_nonfull;
    pthread_cond_t cond_job_done;
} JobScheduler;


void jsInit(int);
int  jsSchedule(Job);
int  jsObtain(Job*);
int  jsBarrier();
void jsStop();
void jsDestroy();

void *job_executor_thread(void *);

#endif
