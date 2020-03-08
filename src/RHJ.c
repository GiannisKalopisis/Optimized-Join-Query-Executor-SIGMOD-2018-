//
// Created by dodor on 02/11/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "RHJ.h"
#include "RHJ_stage1.h"
#include "RHJ_stage2.h"
#include "RHJ_stage3.h"
#include "relations.h"
#include "error_check.h"
#include "hash_functions.h"
#include "result.h"

//#define DEBUG_START
//#define DEBUG_STAGE_1
//#define DEBUG_STAGE_2
//#define DEBUG_STAGE_3

uint64_t * pSumR;
uint64_t * pSumS;
extern uint64_t ** indexBucket;
extern uint64_t ** indexChain;
ResultHead *const res;

ResultHead *RadixHashJoin(Relation *relationR, Relation *relationS){

    //// Stage 1 ////

    Relation *orderedR, *orderedS;

    rInit(&orderedR, relationR->numTuples);
    rInit(&orderedS, relationS->numTuples);


#ifdef DEBUG_START
    printf("=== Printing R before stage 1 ===\n\n");
    for (int i = 0; i < relationR->numTuples; i++){
        printf("RowId of R: %d has payload: %d\n", i+1, rGetPayloadOfIndex(relationR, i));
    }
    printf("\n");
    printf("=== Printing S before stage 1 ===\n\n");
    for (int i = 0; i < relationS->numTuples; i++){
        printf("RowId of S: %d has payload: %d\n", i+1, rGetPayloadOfIndex(relationS, i));
    }
    printf("\n");
#endif

    pSumR = malloc(H1BUCKETS * sizeof(uint64_t));
    NULL_POINTER_ERROR_CHECK(pSumR, __FILE__, __func__, __LINE__);

    pSumS = malloc(H1BUCKETS * sizeof(uint64_t));
    NULL_POINTER_ERROR_CHECK(pSumS, __FILE__, __func__, __LINE__);

    RHJ_stage1(relationR, orderedR, pSumR);
    RHJ_stage1(relationS, orderedS, pSumS);


#ifdef DEBUG_STAGE_1
    printf("=== Printing Ordered R ===\n\n");
    for (int i = 0; i < relationR->numTuples; i++){
        printf("RowId of ordered R: %d has payload: %d\n", i+1, rGetPayloadOfIndex(orderedR, i));
    }
    printf("\n");
    printf("=== Printing Ordered S ===\n\n");
    for (int i = 0; i < relationS->numTuples; i++){
        printf("RowId of ordered S: %d has payload: %d\n", i+1, rGetPayloadOfIndex(orderedS, i));
    }
    printf("\n");
    printf("=== Printing pSumR ===\n\n");
    for (int i = 0; i < H1BUCKETS; i++){
        printf("pSumR with hash value: %d has index: %d\n", i, pSumR[i]);
    }
    printf("\n");
    printf("=== Printing pSumS ===\n\n");
    for (int i = 0; i < H1BUCKETS; i++){
        printf("pSumS with hash value: %d has index: %d\n", i, pSumS[i]);
    }
    printf("\n");
#endif

    //// Stage 2 ////

#ifdef DEBUG_STAGE_2
    printf("=== Choosing smallest relation for stage 2 ===\n\n");
#endif
    // Choose smallest relation for index creation
    Relation *smallestRel, *largestRel;
    uint64_t *smallestPsum, *largestPsum;
    if (orderedR->numTuples <= orderedS->numTuples) {
        smallestRel = orderedR;
        smallestPsum = pSumR;
        largestRel = orderedS;
        largestPsum = pSumS;

#ifdef DEBUG_STAGE_2
        printf("--> Relation R chosen (smallest) \n\n");
#endif
    }
    else {
        smallestRel = orderedS;
        smallestPsum = pSumS;
        largestRel = orderedR;
        largestPsum = pSumR;
#ifdef DEBUG_STAGE_2
        printf("--> Relation S chosen (smallest) \n\n");
#endif
    }

    RHJ_stage2(smallestRel, smallestPsum, &indexBucket, &indexChain);


#ifdef DEBUG_STAGE_2
    printf("=== Printing buckets after index initialization ===\n\n");
    for (int i = 0; i < H1BUCKETS; i++){
        for (int j = 0; j < H2BUCKETS; j++){
            printf("%d ", indexBucket[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    printf("=== Printing chains after index initialization ===\n\n");
    uint64_t chainSize;
    for (int i = 0; i < H1BUCKETS; i++){
        if (i + 1 == H1BUCKETS){
            chainSize = smallestRel->numTuples - smallestPsum[i];
        }
        else {
            chainSize = smallestPsum[i+1] - smallestPsum[i];
        }
        printf("--> ");
        for (int j = 0; j < chainSize; j++){
            printf("%d ", indexChain[i][j]);
        }
        printf("\n");
    }
#endif

    //// Stage 3 ////

    resultRestart(res);
    RHJ_stage3(res, smallestRel, largestRel, smallestPsum, largestPsum, indexChain, indexBucket);


#ifdef DEBUG_STAGE_3
    printf("=== Printing Results ===\n\n");
    resultPrintAll(res);
#endif

    //// Memory Deallocation ////

    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);

    rDestroy(orderedR);
    rDestroy(orderedS);

    free(pSumR);
    free(pSumS);

    return res;
}
