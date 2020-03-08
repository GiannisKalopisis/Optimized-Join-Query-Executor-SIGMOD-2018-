#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "RHJ_stage2.h"
#include "hash_functions.h"
#include "error_check.h"
#include "../include/job_scheduler.h"

extern JobScheduler *const jobScheduler;

uint64_t ** indexBucket;
uint64_t ** indexChain;
// Globals for InitializeBucketJob and InitializeIndexJob
uint64_t **indexBuckets;
uint64_t **indexChains;
Relation *curRel;


void RHJ_stage2(Relation *relationArray, uint64_t *Psum, uint64_t ***indexBucket, uint64_t ***indexChain){

    *indexBucket = malloc(H1BUCKETS * sizeof(uint64_t *));
    NULL_POINTER_ERROR_CHECK(*indexBucket, __FILE__, __func__, __LINE__);

    for (int i = 0; i < H1BUCKETS; i++){
        (*indexBucket)[i] = malloc(H2BUCKETS * sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK((*indexBucket)[i], __FILE__, __func__, __LINE__);
    }

    *indexChain = malloc(H1BUCKETS * sizeof(uint64_t *));
    NULL_POINTER_ERROR_CHECK(*indexChain, __FILE__, __func__, __LINE__);

    chainCreation(relationArray, Psum, indexChain);
    //initializeBuckets(*indexBucket);
    initializationIndex(relationArray, Psum, *indexBucket, *indexChain);
}



//allocating index
void chainCreation(Relation *relationArray, uint64_t *Psum, uint64_t ***indexChain){

    int i, chainSize;
    for (i = 0; i < H1BUCKETS; i++) {
        if (i == (H1BUCKETS - 1)) {
            chainSize = relationArray->numTuples - Psum[i];
        } else {
            chainSize = Psum[i + 1] - Psum[i];
        }
        (*indexChain)[i] = malloc(chainSize * sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK((*indexChain)[i], __FILE__, __func__, __LINE__);
    }
}

/*
void InitializeBucketJob(void *args) {

    // Input: int bucketIndex

    int i, bucketIndex;

    bucketIndex = ((int*) args)[0];

    uint64_t *const*const indexBucketsLocal = indexBuckets;

    if (H1BUCKETS < H2BUCKETS) {
        for (i = 0; i < H2BUCKETS; i++){
            indexBucketsLocal[bucketIndex][i] = 0;
        }
    }
    else {
        for (i = 0; i < H1BUCKETS; i++){
            indexBucketsLocal[i][bucketIndex] = 0;
        }
    }

}
*/

void initializeBuckets(uint64_t **indexBucket){

    /*
    // Saving global variables locally for less assembly code
    JobScheduler *const jobSchedulerLocal = jobScheduler;

    indexBuckets = indexBucket;

    Job curJob;
    int i, numJobs;

    if (H1BUCKETS < H2BUCKETS) {
        numJobs = H1BUCKETS;
    }
    else {
        numJobs = H2BUCKETS;
    }
    */



    /*
    jobSchedulerLocal->numJobs = numJobs;

    curJob.jobRoutine = InitializeBucketJob;
    for (i = 0; i < numJobs; i++){
        curJob.arguments = malloc(sizeof(int));
        NULL_POINTER_ERROR_CHECK(curJob.arguments, __FILE__, __func__, __LINE__);

        memcpy(curJob.arguments, &i, sizeof(int));

        jsSchedule(curJob);
    }


    jsBarrier();
     */
}


void InitializeIndexJob(void *args) {

    // Input: int bucketIndex, int startIndex, int endIndex

    int i, index, bucketIndex, startIndex, endIndex;

    bucketIndex = ((int*) args)[0];
    startIndex = ((int*) args)[1];
    endIndex = ((int*) args)[2];

    // Saving global variables locally for less assembly code
    const Relation *const curRelLocal = curRel;
    uint64_t *const*const indexBucketsLocal = indexBuckets;
    uint64_t *const*const indexChainsLocal = indexChains;

    memset(indexBucketsLocal[bucketIndex], 0, H2BUCKETS * sizeof(uint64_t));

    uint64_t hashValue2;
    index = 0; // Index for current chain and bucket
    for (i = startIndex; i <= endIndex; i++) {
        hashValue2 = HASH_FUNCTION_2(curRelLocal->tuples[i].payload);

        indexChainsLocal[bucketIndex][index] = indexBucketsLocal[bucketIndex][hashValue2];
        indexBucketsLocal[bucketIndex][hashValue2] = index + 1;
        index++;
    }
}

//initializing index with values
void initializationIndex(Relation *relationArray, uint64_t *Psum, uint64_t **indexBucket, uint64_t **indexChain){

    // Saving global variables locally for less assembly code
    JobScheduler *const jobSchedulerLocal = jobScheduler;

    curRel = relationArray;
    indexBuckets = indexBucket;
    indexChains = indexChain;

    Job curJob;
    int i, numJobs, startIndex, endIndex;

    numJobs = H1BUCKETS;
    jobSchedulerLocal->numJobs = numJobs;

    // Initialize index using threads

    curJob.jobRoutine = InitializeIndexJob;
    for (i = 0; i < numJobs; i++) {
        curJob.arguments = malloc(3*sizeof(int));

        startIndex = Psum[i];

        //end of Psum Array
        if ( i == numJobs - 1 ){
            endIndex = relationArray->numTuples - 1;
        }
            //Psum have more hash values
        else {
            endIndex = Psum[i + 1] - 1;
        }

        memcpy(curJob.arguments, &i, sizeof(int));
        memcpy(curJob.arguments + sizeof(int), &startIndex, sizeof(int));
        memcpy(curJob.arguments + 2*sizeof(int), &endIndex, sizeof(int));

        jsSchedule(curJob);

    }

    jsBarrier();
}


void deleteInnerChain(uint64_t **indexChain) {

    int i;
    for (i = 0; i < H1BUCKETS; i++) {
        free(indexChain[i]);
    }
}

//deleting index
void deleteIndex(uint64_t **indexBucket, uint64_t **indexChain){

    // Inner chain is free'd in each call of radix hash because it doesnt have a constant size

    int i;
    for (i = 0; i < H1BUCKETS; i++) {
        free(indexBucket[i]);
    }
    free(indexChain);
    free(indexBucket);
}