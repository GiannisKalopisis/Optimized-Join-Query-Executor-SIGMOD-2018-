#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "RHJ_stage1.h"
#include "../include/relations.h"
#include "hash_functions.h"
#include "error_check.h"
#include "../include/job_scheduler.h"

extern JobScheduler *const jobScheduler;

// Globals for HistogramJob and PartitionJob
Relation *curRel;
Relation *curOrderRel;

uint64_t **histogram;
uint64_t **positions;
uint64_t *curpSum;

void HistogramJob(void *args) {

    // Input: int hist_index, int start_index, int end_index

    int i, hist_index, start_index, end_index;

    hist_index = ((int*) args)[0];
    start_index = ((int*) args)[1];
    end_index = ((int*) args)[2];

    // Saving global variables locally for less assembly code
    const Relation *const curRelLocal = curRel;
    uint64_t *const*const histogramLocal = histogram;

    for (i = 0; i < H1BUCKETS; i++) {
        histogramLocal[hist_index][i] = 0;
    }

    uint64_t hashValue;
    for (i = start_index; i <= end_index; i++) {
        hashValue = HASH_FUNCTION_1(curRelLocal->tuples[i].payload);
        histogramLocal[hist_index][hashValue]++;
    }
}


void PartitionJob(void *args) {

    // Input: int hist_index, int start_index, int end_index

    int i, j, hist_index, start_index, end_index;

    hist_index = ((int*) args)[0];
    start_index = ((int*) args)[1];
    end_index = ((int*) args)[2];

    // Saving global variables locally for less assembly code
    const Relation *const curRelLocal = curRel;
    const Relation *const curOrderRelLocal = curOrderRel;

    uint64_t *const*const histogramLocal = histogram;
    uint64_t *const*const positionsLocal = positions;
    const uint64_t *const curpSumLocal = curpSum;

    for (i = 0; i < H1BUCKETS; i++) {
        positionsLocal[hist_index][i] = curpSumLocal[i];
        for (j = 0; j < hist_index; j++) {
            positionsLocal[hist_index][i] += histogramLocal[j][i];
        }
    }

    uint64_t hashValue, position;
    for (i = start_index; i <= end_index; i++) {
        hashValue = HASH_FUNCTION_1(curRelLocal->tuples[i].payload);
        position = positionsLocal[hist_index][hashValue];
        curOrderRelLocal->tuples[position] = curRelLocal->tuples[i];
        positionsLocal[hist_index][hashValue]++;
    }
}


void RHJ_stage1(Relation *relation, Relation *orderedRel, uint64_t *pSum) {

    // Saving global variables locally for less assembly code
    JobScheduler *const jobSchedulerLocal = jobScheduler;
    uint64_t *const*const histogramLocal = histogram;

    curRel = relation;
    curOrderRel = orderedRel;
    curpSum = pSum;

    Job curJob;
    int i, j, numJobs, tuplesPerJob, tuplesLastJob, startIndex, endIndex;

    // If less tuples than numThreads, create numTuple jobs instead
    if (relation->numTuples < NUM_THREADS) {
        jobSchedulerLocal->numJobs = relation->numTuples;
    }
    else {
        jobSchedulerLocal->numJobs = NUM_THREADS;
    }
    numJobs = jobSchedulerLocal->numJobs;

    tuplesPerJob = relation->numTuples / numJobs;
    tuplesLastJob = tuplesPerJob + (relation->numTuples % numJobs);

    for (i = 0; i < NUM_THREADS; i++) {
        histogram[i] = malloc(H1BUCKETS * sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK(histogram[i], __FILE__, __func__, __LINE__);
    }

    // Create histogram using threads

    curJob.jobRoutine = HistogramJob;
    for (i = 0; i < numJobs; i++) {
        curJob.arguments = malloc(3*sizeof(int));
        NULL_POINTER_ERROR_CHECK(curJob.arguments, __FILE__, __func__, __LINE__);

        startIndex = i * tuplesPerJob;
        // If last job
        if (i == numJobs - 1) {
            endIndex = startIndex + tuplesLastJob - 1;
        }
        else {
            endIndex = startIndex + tuplesPerJob -1;
        }

        memcpy(curJob.arguments, &i, sizeof(int));
        memcpy(curJob.arguments + sizeof(int), &startIndex, sizeof(int));
        memcpy(curJob.arguments + 2*sizeof(int), &endIndex, sizeof(int));

        jsSchedule(curJob);
        pthread_cond_broadcast(&jobSchedulerLocal->cond_nonempty);
    }

    jsBarrier();

    // Create pSum

    pSum[0] = 0;
    for (i = 1; i < H1BUCKETS; i++) {
        pSum[i] = pSum[i-1];
        for (j = 0; j < numJobs; ++j) {
            pSum[i] += histogramLocal[j][i-1];
        }
    }


    for (i = 0; i < NUM_THREADS; i++) {
        positions[i] = malloc(H1BUCKETS * sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK(positions[i], __FILE__, __func__, __LINE__);
    }

    // Create ordered Relation using threads

    jobSchedulerLocal->numJobs = numJobs;
    curJob.jobRoutine = PartitionJob;
    for (i = 0; i < numJobs; i++) {
        curJob.arguments = malloc(3*sizeof(int));
        NULL_POINTER_ERROR_CHECK(curJob.arguments, __FILE__, __func__, __LINE__);

        startIndex = i * tuplesPerJob;
        // If last job
        if (i == numJobs - 1) {
            endIndex = startIndex + tuplesLastJob - 1;
        }
        else {
            endIndex = startIndex + tuplesPerJob -1;
        }

        memcpy(curJob.arguments, &i, sizeof(int));
        memcpy(curJob.arguments + sizeof(int), &startIndex, sizeof(int));
        memcpy(curJob.arguments + 2*sizeof(int), &endIndex, sizeof(int));

        jsSchedule(curJob);
    }

    jsBarrier();

    for (i = 0; i < NUM_THREADS; i++) {
        free(histogram[i]);
        free(positions[i]);
    }
}