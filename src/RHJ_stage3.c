#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "RHJ_stage3.h"
#include "../include/relations.h"
#include "../include/result.h"
#include "../include/error_check.h"
#include "../include/hash_functions.h"
#include "../include/job_scheduler.h"

extern JobScheduler *const jobScheduler;

// Globals for JoinJob
Relation *largeRel;
Relation *smallRel;
uint64_t *smallpSum;
uint64_t **indexBuckets;
uint64_t **indexChains;
ResultHead **results;

void JoinJob(void *args) {

    // Input: int bucketIndex, int largeStartIndex, int largeEndIndex

    int i, index, indexNext, bucketIndex, largeStartIndex, largeEndIndex;

    bucketIndex = ((int*) args)[0];
    largeStartIndex = ((int*) args)[1];
    largeEndIndex = ((int*) args)[2];

    // Saving global variables locally for less assembly code
    const Relation *const largeRelLocal = largeRel;
    const Relation *const smallRelLocal = smallRel;
    const uint64_t *const smallpSumLocal = smallpSum;
    uint64_t *const*const indexBucketsLocal = indexBuckets;
    uint64_t *const*const indexChainsLocal = indexChains;
    ResultHead *const*const resultsLocal = results;

    uint64_t hashValue2;
    for (i = largeStartIndex; i <= largeEndIndex; i++) {
        hashValue2 = HASH_FUNCTION_2( largeRelLocal->tuples[i].payload );
        index = indexBucketsLocal[bucketIndex][hashValue2];
        while ( index != 0 ) {
            indexNext = smallpSumLocal[bucketIndex] + (index - 1);
            if ( smallRelLocal->tuples[indexNext].payload == largeRelLocal->tuples[i].payload ) {
                resultAppend(resultsLocal[bucketIndex], smallRelLocal->tuples[indexNext].rowId, largeRelLocal->tuples[i].rowId);
            }
            index = indexChainsLocal[bucketIndex][index-1];
        }
    }
}


void RHJ_stage3(ResultHead *res, Relation *smallestRel, Relation *largestRel, uint64_t *smallestPsum,
        uint64_t *largestPsum, uint64_t **indexChain, uint64_t **indexBucket) {

    /* for (i = 0 ; i < H1BUCKETS; i++){
     * get indexes of smallOrdered from smallPsum
     * do loop for that bucket
     * for each element calculate the h2 value
     *    and search at h2 value index of that bucket and check
     *    chains (careful of the + 1 values on indexes)
     *    and check the elements which have same h2, do
     *    comparison, if equal add them to result list
     */

    results = malloc(H1BUCKETS * sizeof(ResultHead*));
    NULL_POINTER_ERROR_CHECK(results, __FILE__, __func__, __LINE__);
    for (int i = 0; i < H1BUCKETS; i++) {
        resultInit(&results[i]);
    }

    // Saving global variables locally for less assembly code
    JobScheduler *const jobSchedulerLocal = jobScheduler;
    ResultHead *const*const resultsLocal = results;

    largeRel = largestRel;
    smallRel = smallestRel;
    smallpSum = smallestPsum;
    indexBuckets = indexBucket;
    indexChains = indexChain;

    Job curJob;
    int i, j, numJobs, largeStartIndex, largeEndIndex;

    numJobs = H1BUCKETS;
    jobSchedulerLocal->numJobs = H1BUCKETS;

    // Join buckets using threads

    curJob.jobRoutine = JoinJob;
    for (i = 0; i < numJobs; i++) {
        curJob.arguments = malloc(3*sizeof(int));
        NULL_POINTER_ERROR_CHECK(curJob.arguments, __FILE__, __func__, __LINE__);

        largeStartIndex = largestPsum[i];

        if ( i == numJobs - 1 ){
            largeEndIndex = largestRel->numTuples - 1;
        }
        else {
            largeEndIndex = largestPsum[i + 1] - 1;
        }

        memcpy(curJob.arguments, &i, sizeof(int));
        memcpy(curJob.arguments + sizeof(int), &largeStartIndex, sizeof(int));
        memcpy(curJob.arguments + 2*sizeof(int), &largeEndIndex, sizeof(int));

        jsSchedule(curJob);
    }

    // Connect results all into res

    jsBarrier();

    Result *cur;
    for (i = 0; i < numJobs; i++) {
        cur = resultsLocal[i]->first;
        while (cur != NULL) {
            if (cur->numOfTuples == 0) {
                break;
            }
            for (j = 0; j < cur->numOfTuples; j++) {
                resultAppend(res, cur->buffer[j].rowId1, cur->buffer[j].rowId2);
            }
            cur = cur->next;
        }
    }

    for (i = 0; i < H1BUCKETS; i++) {
        resultDestroy(results[i]);
    }
    free(results);
}