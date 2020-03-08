#include <stdlib.h>
#include <pthread.h>

#include "../include/error_check.h"
#include "../include/job_scheduler.h"

JobScheduler *jobScheduler;

void jsInit(int numThreads) {

    jobScheduler = malloc(sizeof(JobScheduler));
    NULL_POINTER_ERROR_CHECK(jobScheduler, __FILE__, __func__, __LINE__);

    // Circular Queue creation

    JobScheduler *jobSchedulerLocal = jobScheduler;

    jobSchedulerLocal->queue = malloc(JOB_QUEUE_SIZE * sizeof(Job));
    NULL_POINTER_ERROR_CHECK(jobSchedulerLocal->queue, __FILE__, __func__, __LINE__);

    jobSchedulerLocal->queueFront = 0;
    jobSchedulerLocal->queueRear = -1;
    jobSchedulerLocal->queueCount = 0;

    jobSchedulerLocal->numThreads = numThreads;

    jobSchedulerLocal->numJobs = 0;

    // Mutex and Condition variables creation

    pthread_mutex_init(&jobSchedulerLocal->queuelock, 0);
    pthread_mutex_init(&jobSchedulerLocal->joblock, 0);

    pthread_cond_init(&jobSchedulerLocal->cond_nonempty, 0);
    pthread_cond_init(&jobSchedulerLocal->cond_nonfull, 0);
    pthread_cond_init(&jobSchedulerLocal->cond_job_done, 0);

    // Thread creation

    jobSchedulerLocal->threads = malloc(numThreads * sizeof(pthread_t));
    NULL_POINTER_ERROR_CHECK(jobSchedulerLocal->threads, __FILE__, __func__, __LINE__);

    int i;
    for (i = 0; i < numThreads; i++) {
        pthread_create(&jobSchedulerLocal->threads[i], NULL, job_executor_thread, NULL);
    }
}


int jsSchedule(Job job) {

    // Add a job inside the queue

    JobScheduler *jobSchedulerLocal = jobScheduler;

    int error;

    // Lock
    if ( (error = pthread_mutex_lock(&jobSchedulerLocal->queuelock)) != 0) {
        return error;
    }

    // If queue is full then wait until there is space to add it
    while (jobSchedulerLocal->queueCount == JOB_QUEUE_SIZE) {
        pthread_cond_wait(&jobSchedulerLocal->cond_nonfull, &jobSchedulerLocal->queuelock);
    }

    jobSchedulerLocal->queueRear = (jobSchedulerLocal->queueRear + 1) % JOB_QUEUE_SIZE;
    jobSchedulerLocal->queue[jobSchedulerLocal->queueRear].jobRoutine = job.jobRoutine;
    jobSchedulerLocal->queue[jobSchedulerLocal->queueRear].arguments = job.arguments;

    jobSchedulerLocal->queueCount++;

    pthread_cond_broadcast(&jobSchedulerLocal->cond_nonempty);

    // Unlock
    if ( (error = pthread_mutex_unlock(&jobSchedulerLocal->queuelock)) != 0) {
        return error;
    }

    return 0;
}

int jsObtain(Job *job) {

    // Get a job from the queue

    JobScheduler *jobSchedulerLocal = jobScheduler;

    int error;

    // Lock
    if ( (error = pthread_mutex_lock(&jobSchedulerLocal->queuelock)) != 0) {
        return error;
    }

    // If queue is empty then wait until there is a job to do
    while (jobSchedulerLocal->queueCount == 0) {
        pthread_cond_wait(&jobSchedulerLocal->cond_nonempty, &jobSchedulerLocal->queuelock);
    }

    job->jobRoutine = jobSchedulerLocal->queue[jobSchedulerLocal->queueFront].jobRoutine;
    job->arguments = jobSchedulerLocal->queue[jobSchedulerLocal->queueFront].arguments;

    jobSchedulerLocal->queueFront = (jobSchedulerLocal->queueFront + 1) % JOB_QUEUE_SIZE;
    jobSchedulerLocal->queueCount--;

    pthread_cond_broadcast(&jobSchedulerLocal->cond_nonfull);

    // Unlock
    if ( (error = pthread_mutex_unlock(&jobSchedulerLocal->queuelock)) != 0) {
        return error;
    }

    return 0;
}


int jsBarrier() {

    // Wait until queue is empty ( all jobs are done )

    JobScheduler *jobSchedulerLocal = jobScheduler;

    int error;

    // Lock
    if ( (error = pthread_mutex_lock(&jobSchedulerLocal->joblock)) != 0) {
        return error;
    }

    while (jobSchedulerLocal->numJobs > 0) {
        pthread_cond_wait(&jobSchedulerLocal->cond_job_done, &jobSchedulerLocal->joblock);
    }

    // Unlock
    if ( (error = pthread_mutex_unlock(&jobSchedulerLocal->joblock)) != 0) {
        return error;
    }

    return 0;
}


void jsStop() {

    int i;

    JobScheduler *jobSchedulerLocal = jobScheduler;

    jsBarrier();

    Job terminalJob;
    terminalJob.jobRoutine = NULL;
    for (i = 0; i < jobSchedulerLocal->numThreads; i++) {
        // Place something in queue so that a thread that reads it terminates
        jsSchedule(terminalJob);
        pthread_cond_broadcast(&jobSchedulerLocal->cond_nonempty);
    }

    for (i = 0; i < jobSchedulerLocal->numThreads; i++) {
        pthread_join(jobSchedulerLocal->threads[i], NULL);
    }
}


void jsDestroy() {

    JobScheduler *jobSchedulerLocal = jobScheduler;

    // Mutex and Condition variables deallocation

    pthread_mutex_destroy(&jobSchedulerLocal->queuelock);
    pthread_mutex_destroy(&jobSchedulerLocal->joblock);

    pthread_cond_destroy(&jobSchedulerLocal->cond_nonempty);
    pthread_cond_destroy(&jobSchedulerLocal->cond_nonfull);
    pthread_cond_destroy(&jobSchedulerLocal->cond_job_done);

    // Other variable deallocations

    free(jobSchedulerLocal->threads);
    free(jobSchedulerLocal->queue);

    free(jobSchedulerLocal);
}


void *job_executor_thread(void *ptr) {

    JobScheduler *jobSchedulerLocal = jobScheduler;

    Job currentJob;
    while(1) {

        jsObtain(&currentJob);


        // Check if element read from queue means termination, if yes, break from loop
        if (currentJob.jobRoutine == NULL) {
            break;
        }

        // If not, execute job

        (*(currentJob.jobRoutine))(currentJob.arguments);

        free(currentJob.arguments);

        // Lock
        if ( pthread_mutex_lock(&jobSchedulerLocal->joblock) != 0) {
            return NULL;
        }

        jobSchedulerLocal->numJobs--;
        if (jobSchedulerLocal->numJobs == 0) {
            pthread_cond_signal(&jobSchedulerLocal->cond_job_done);
        }

        // Unlock
        if ( pthread_mutex_unlock(&jobSchedulerLocal->joblock) != 0) {
            return NULL;
        }
    }

    return NULL;
}