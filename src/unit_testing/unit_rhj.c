#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../include/unit_rhj.h"
#include "../../include/RHJ.h"
#include "../../include/RHJ_stage1.h"
#include "../../include/RHJ_stage2.h"
#include "../../include/RHJ_stage3.h"
#include "../../include/error_check.h"
#include "../../include/hash_functions.h"
#include "../../include/general_functions.h"
#include "../../include/relations.h"
#include "../../include/result.h"
#include "CUnit/Basic.h"

extern uint64_t **indexBucket;
extern uint64_t **indexChain;
extern ResultHead *res;

void testRHJ_stage1(void) {

    // Assuming that we have H1BITS > 3 and h1 = H1BITS less significant bits
    // so H1BUCKETS > 10, if we want it to work for every H1BITS we have to
    // create a histogram here to compare values

    Relation rel, orderedRel;
    rel.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(rel.tuples, __FILE__, __func__, __LINE__);
    orderedRel.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(orderedRel.tuples, __FILE__, __func__, __LINE__);

    uint64_t pSum[H1BUCKETS];
    int i;

    // First Test - ascending payloads //

    /*  Input   Relation | Output  orderRel |     Output pSum
     *  (rowId, payload) | (rowId, payload) |  (hash1value, index)
     *      (1, 0)       |     (1, 0)       |      (0, 0)
     *      (2, 1)       |     (2, 1)       |      (1, 1)
     *      (3, 2)       |     (3, 2)       |      (2, 2)
     *      (4, 3)       |     (4, 3)       |      (3, 3)
     *      (5, 4)       |     (5, 4)       |      (4, 4)
     *      (6, 5)       |     (6, 5)       |      (5, 5)
     *      (7, 6)       |     (7, 6)       |      (6, 6)
     *      (8, 7)       |     (8, 7)       |      (7, 7)
     *      (9, 8)       |     (9, 8)       |      (8, 8)
     *     (10, 9)       |    (10, 9)       |      (9, 9)
     *        -          |       -          |     (10, 10)
     *        -          |       -          |      ......
     *        -          |       -          |  (numBuckets - 1, 10)
     */


    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = i;
    }
    rel.numTuples = 10;

    // This test should return orderedRel == rel
    RHJ_stage1(&rel, &orderedRel, pSum);

    for (i = 0; i < 10; ++i) {
        CU_ASSERT(orderedRel.tuples[i].rowId == i + 1);
        CU_ASSERT(orderedRel.tuples[i].payload == i);
    }

    for (i = 0; i < 20; i++) {
        if (i > 9) {
            CU_ASSERT(pSum[i] == 10);
            continue;
        }
        CU_ASSERT(pSum[i] == i);
    }

    // Second Test - descending payloads //

    /*  Input   Relation | Output  orderRel |     Output pSum
     *  (rowId, payload) | (rowId, payload) |  (hash1value, index)
     *      (1, 9)       |    (10, 0)       |      (0, 0)
     *      (2, 8)       |     (9, 1)       |      (1, 1)
     *      (3, 7)       |     (8, 2)       |      (2, 2)
     *      (4, 6)       |     (7, 3)       |      (3, 3)
     *      (5, 5)       |     (6, 4)       |      (4, 4)
     *      (6, 4)       |     (5, 5)       |      (5, 5)
     *      (7, 3)       |     (4, 6)       |      (6, 6)
     *      (8, 2)       |     (3, 7)       |      (7, 7)
     *      (9, 1)       |     (2, 8)       |      (8, 8)
     *     (10, 0)       |     (1, 9)       |      (9, 9)
     *        -          |       -          |     (10, 10)
     *        -          |       -          |      ......
     *        -          |       -          |  (numBuckets - 1, 10)
     */


    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = 9 - i;
    }
    rel.numTuples = 10;

    // This test should return orderedRel == "reversed" rel
    RHJ_stage1(&rel, &orderedRel, pSum);

    for (i = 0; i < 10; ++i) {
        CU_ASSERT(orderedRel.tuples[i].rowId == 10 - i);
        CU_ASSERT(orderedRel.tuples[i].payload == i);
    }

    for (i = 0; i < 20; i++) {
        if (i > 9) {
            CU_ASSERT(pSum[i] == 10);
            continue;
        }
        CU_ASSERT(pSum[i] == i);
    }

    // Third Test - multiple same payloads, ascending order //

    /*  Input   Relation | Output  orderRel |     Output pSum
     *  (rowId, payload) | (rowId, payload) |  (hash1value, index)
     *      (1, 0)       |     (1, 0)       |      (0, 0)
     *      (2, 0)       |     (2, 0)       |      (1, 2)
     *      (3, 1)       |     (3, 1)       |      (2, 4)
     *      (4, 1)       |     (4, 1)       |      (3, 6)
     *      (5, 2)       |     (5, 2)       |      (4, 8)
     *      (6, 2)       |     (6, 2)       |      (5, 10)
     *      (7, 3)       |     (7, 3)       |      (6, 10)
     *      (8, 3)       |     (8, 3)       |      (7, 10)
     *      (9, 4)       |     (9, 4)       |      (8, 10)
     *     (10, 4)       |    (10, 4)       |      (9, 10)
     *        -          |       -          |     (10, 10)
     *        -          |       -          |      ......
     *        -          |       -          |  (numBuckets - 1, 10)
     */

    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = i/2;
    }
    rel.numTuples = 10;

    RHJ_stage1(&rel, &orderedRel, pSum);

    for (i = 0; i < 10; i += 2) {

        CU_ASSERT(orderedRel.tuples[i].rowId == i + 1);
        CU_ASSERT(orderedRel.tuples[i].payload == i/2);
        CU_ASSERT(orderedRel.tuples[i + 1].rowId == i + 2);
        CU_ASSERT(orderedRel.tuples[i + 1].payload == i/2);
    }

    for (i = 0; i < 20; i++) {
        if (i > 5) {
            CU_ASSERT(pSum[i] == 10);
            continue;
        }
        CU_ASSERT(pSum[i] == i*2);
    }

    // Fourth Test - multiple same payloads, descending order //

    /*  Input   Relation | Output  orderRel |     Output pSum
     *  (rowId, payload) | (rowId, payload) |  (hash1value, index)
     *      (1, 4)       |     (9, 0)       |      (0, 0)
     *      (2, 4)       |    (10, 0)       |      (1, 2)
     *      (3, 3)       |     (7, 1)       |      (2, 4)
     *      (4, 3)       |     (8, 1)       |      (3, 6)
     *      (5, 2)       |     (5, 2)       |      (4, 8)
     *      (6, 2)       |     (6, 2)       |      (5, 10)
     *      (7, 1)       |     (3, 3)       |      (6, 10)
     *      (8, 1)       |     (4, 3)       |      (7, 10)
     *      (9, 0)       |     (1, 4)       |      (8, 10)
     *     (10, 0)       |     (2, 4)       |      (9, 10)
     *        -          |       -          |     (10, 10)
     *        -          |       -          |      ......
     *        -          |       -          |  (numBuckets - 1, 10)
     */

    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = (9 - i)/2;
    }
    rel.numTuples = 10;

    RHJ_stage1(&rel, &orderedRel, pSum);

    for (i = 0; i < 10; i += 2) {

        CU_ASSERT(orderedRel.tuples[i].rowId == 10 - i - 1);
        CU_ASSERT(orderedRel.tuples[i].payload == i/2);
        CU_ASSERT(orderedRel.tuples[i + 1].rowId == 10 - i);
        CU_ASSERT(orderedRel.tuples[i + 1].payload == i/2);
    }

    for (i = 0; i < 20; i++) {
        if (i > 5) {
            CU_ASSERT(pSum[i] == 10);
            continue;
        }
        CU_ASSERT(pSum[i] == i*2);
    }

    // Fifth Test - all same payloads //

    /*  Input   Relation | Output  orderRel |     Output pSum
     *  (rowId, payload) | (rowId, payload) |  (hash1value, index)
     *      (1, 7)       |     (1, 7)       |      (0, 0)
     *      (2, 7)       |     (2, 7)       |      (1, 0)
     *      (3, 7)       |     (3, 7)       |      (2, 0)
     *      (4, 7)       |     (4, 7)       |      (3, 0)
     *      (5, 7)       |     (5, 7)       |      (4, 0)
     *      (6, 7)       |     (6, 7)       |      (5, 0)
     *      (7, 7)       |     (7, 7)       |      (6, 0)
     *      (8, 7)       |     (8, 7)       |      (7, 0)
     *      (9, 7)       |     (9, 7)       |      (8, 10)
     *     (10, 7)       |    (10, 7)       |      (9, 10)
     *        -          |       -          |     (10, 10)
     *        -          |       -          |      ......
     *        -          |       -          |  (numBuckets - 1, 10)
     */

    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = 7;
    }
    rel.numTuples = 10;

    RHJ_stage1(&rel, &orderedRel, pSum);

    for (i = 0; i < 10; i++) {

        CU_ASSERT(orderedRel.tuples[i].rowId == i + 1);
        CU_ASSERT(orderedRel.tuples[i].payload == 7);
    }

    for (i = 0; i < 20; i++) {
        if (i < 8) {
            CU_ASSERT(pSum[i] == 0);
            continue;
        }
        CU_ASSERT(pSum[i] == 10);
    }

    free(rel.tuples);
    free(orderedRel.tuples);

}

void testIndexFunctions(void) {

    Relation rel, orderedRel;
    rel.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(rel.tuples, __FILE__, __func__, __LINE__);
    orderedRel.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(orderedRel.tuples, __FILE__, __func__, __LINE__);

    uint64_t pSum[H1BUCKETS];

    int i;

    // First Test - ascending payloads //

    /*  Input   orderRel | Output IndexChain |       Output IndexBucket
     *  (rowId, payload) |                   |
     *      (1, 0)       |      [ 0 ]    <-  | [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (2, 1)       |      [ 0 ]    <-  | [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (3, 2)       |      [ 0 ]    <-  | [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (4, 3)       |      [ 0 ]    <-  | [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, ...]
     *      (5, 4)       |      [ 0 ]    <-  | [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, ...]
     *      (6, 5)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, ...]
     *      (7, 6)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 1, 0, 0, 0, ...]
     *      (8, 7)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, ...]
     *      (9, 8)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 0, 0, 1, 0, ...]
     *     (10, 9)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, ...]
     *        -          |        -          |                -
     */


    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = i;
    }
    rel.numTuples = 10;
    orderedRel.numTuples = rel.numTuples;

    // This test should return orderedRel == rel
    RHJ_stage1(&rel, &orderedRel, pSum);

    /* === Stage 2 testing === */

    indexBucket = malloc(H1BUCKETS * sizeof(uint64_t *));
    NULL_POINTER_ERROR_CHECK(indexBucket, __FILE__, __func__, __LINE__);

    for (int i = 0; i < H1BUCKETS; i++){
        indexBucket[i] = malloc(H2BUCKETS * sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK(indexBucket[i], __FILE__, __func__, __LINE__);
    }

    indexChain = malloc(H1BUCKETS * sizeof(uint64_t *));
    NULL_POINTER_ERROR_CHECK(indexChain, __FILE__, __func__, __LINE__);

    chainCreation(&orderedRel, pSum, &indexChain);

    CU_ASSERT_PTR_NOT_NULL(indexBucket);
    CU_ASSERT_PTR_NOT_NULL(indexChain);

    //initializeBuckets(indexBucket);

    /*
    for (int j = 0; j < H1BUCKETS; ++j) {
        for (int k = 0; k < H2BUCKETS; ++k) {
            if ((j == 0) || (j == (H1BUCKETS/2)) || (j == (H1BUCKETS - 1))) {
                if ((k == 0) || (k == (H2BUCKETS/2)) || (k == (H2BUCKETS - 1))) {
                    CU_ASSERT(indexBucket[j][k] == 0);
                }
            }
        }
    }
    */

    initializationIndex(&orderedRel, pSum, indexBucket, indexChain);

    /*
    for (int l = 0; l < 10; ++l) {
        for (int j = 0; j < 10 ; ++j) {
            if (j != l) {
                CU_ASSERT(indexBucket[l][j] == 0);
            }
        }
        CU_ASSERT(indexBucket[l][l] == 1);
        CU_ASSERT(indexChain[l][0] == 0);
    }
    */
    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);

    // Fourth Test - multiple same payloads, descending order //

    /*  Input   orderRel | Output IndexChain |       Output IndexBucket
     *  (rowId, payload) |                   |
     *      (9, 0)       |      [0, 1]   <-  | [2, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *     (10, 0)       |      [0, 1]   <-  | [0, 2, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (7, 1)       |      [0, 1]   <-  | [0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (8, 1)       |      [0, 1]   <-  | [0, 0, 0, 2, 0, 0, 0, 0, 0, 0, ...]
     *      (5, 2)       |      [0, 1]   <-  | [0, 0, 0, 0, 2, 0, 0, 0, 0, 0, ...]
     *      (6, 2)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (3, 3)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (4, 3)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (1, 4)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (2, 4)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *        -          |        -          |                -
     */


    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = (9 - i)/2;
    }
    rel.numTuples = 10;
    orderedRel.numTuples = rel.numTuples;

    RHJ_stage1(&rel, &orderedRel, pSum);

    /* === Stage 2 testing === */

    indexBucket = malloc(H1BUCKETS * sizeof(uint64_t *));
    NULL_POINTER_ERROR_CHECK(indexBucket, __FILE__, __func__, __LINE__);

    for (int i = 0; i < H1BUCKETS; i++){
        (indexBucket)[i] = malloc(H2BUCKETS * sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK((indexBucket)[i], __FILE__, __func__, __LINE__);
    }

    indexChain = malloc(H1BUCKETS * sizeof(uint64_t *));
    NULL_POINTER_ERROR_CHECK(indexChain, __FILE__, __func__, __LINE__);

    chainCreation(&orderedRel, pSum, &indexChain);

    CU_ASSERT_PTR_NOT_NULL(indexBucket);
    CU_ASSERT_PTR_NOT_NULL(indexChain);

    //initializeBuckets(indexBucket);
    /*
    for (int j = 0; j < H1BUCKETS; ++j) {
        for (int k = 0; k < H2BUCKETS; ++k) {
            if ((j == 0) || (j == (H1BUCKETS/2)) || (j == (H1BUCKETS - 1))) {
                if ((k == 0) || (k == (H2BUCKETS/2)) || (k == (H2BUCKETS - 1))) {
                    CU_ASSERT(indexBucket[j][k] == 0);
                }
            }
        }
    }
    */
    initializationIndex(&orderedRel, pSum, indexBucket, indexChain);

    /*
    for (int l = 0; l < 10; ++l) {
        for (int j = 0; j < 10 ; ++j) {
            if (j != l) {
                CU_ASSERT(indexBucket[l][j] == 0);
            }
        }
        if (l < 5) {
            CU_ASSERT(indexBucket[l][l] == 2);
            CU_ASSERT(indexChain[l][0] == 0);
            CU_ASSERT(indexChain[l][1] == 1);
        }
        else {
            CU_ASSERT(indexBucket[l][l] == 0);
        }

    }
    */

    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);


    free(rel.tuples);
    free(orderedRel.tuples);
}


void testRHJ_stage2(void) {

    // Assuming that we have H1BITS > 3 and h1 = H1BITS less significant bits
    // so H1BUCKETS > 10, if we want it to work for every H1BITS we have to
    // create a histogram here to compare values

    // Same tests inputs with testRHJ_stage1

    Relation rel, orderedRel;
    rel.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(rel.tuples, __FILE__, __func__, __LINE__);
    orderedRel.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(orderedRel.tuples, __FILE__, __func__, __LINE__);

    uint64_t pSum[H1BUCKETS];
    int i;

    // First Test - ascending payloads //

    /*  Input   orderRel | Output IndexChain |       Output IndexBucket
     *  (rowId, payload) |                   |
     *      (1, 0)       |      [ 0 ]    <-  | [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (2, 1)       |      [ 0 ]    <-  | [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (3, 2)       |      [ 0 ]    <-  | [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (4, 3)       |      [ 0 ]    <-  | [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, ...]
     *      (5, 4)       |      [ 0 ]    <-  | [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, ...]
     *      (6, 5)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, ...]
     *      (7, 6)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 1, 0, 0, 0, ...]
     *      (8, 7)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, ...]
     *      (9, 8)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 0, 0, 1, 0, ...]
     *     (10, 9)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, ...]
     *        -          |        -          |                -
     */


    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = i;
    }
    rel.numTuples = 10;
    orderedRel.numTuples = rel.numTuples;

    // This test should return orderedRel == rel
    RHJ_stage1(&rel, &orderedRel, pSum);

    /* === Stage 2 testing === */

    RHJ_stage2(&orderedRel, pSum, &indexBucket, &indexChain);

    /*
    for (int l = 0; l < 10; ++l) {
        for (int j = 0; j < 10 ; ++j) {
            if (j != l) {
                CU_ASSERT(indexBucket[l][j] == 0);
            }
        }
        CU_ASSERT(indexBucket[l][l] == 1);
        CU_ASSERT(indexChain[l][0] == 0);
    }
    */
    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);;


    // Second Test - descending payloads //

    /*  Input   orderRel | Output IndexChain |       Output IndexBucket
     *  (rowId, payload) |                   |
     *     (10, 0)       |      [ 0 ]    <-  | [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (9, 1)       |      [ 0 ]    <-  | [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (8, 2)       |      [ 0 ]    <-  | [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (7, 3)       |      [ 0 ]    <-  | [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, ...]
     *      (6, 4)       |      [ 0 ]    <-  | [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, ...]
     *      (5, 5)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, ...]
     *      (4, 6)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 1, 0, 0, 0, ...]
     *      (3, 7)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, ...]
     *      (2, 8)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 0, 0, 1, 0, ...]
     *      (1, 9)       |      [ 0 ]    <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, ...]
     *        -          |        -          |                -
     */


    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = 9 - i;
    }
    rel.numTuples = 10;
    orderedRel.numTuples = rel.numTuples;

    // This test should return orderedRel == "reversed" rel
    RHJ_stage1(&rel, &orderedRel, pSum);

    /* === Stage 2 testing === */

    RHJ_stage2(&orderedRel, pSum, &indexBucket, &indexChain);
    /*
    for (int l = 0; l < 10; ++l) {
        for (int j = 0; j < 10 ; ++j) {
            if (j != l) {
                CU_ASSERT(indexBucket[l][j] == 0);
            }
        }
        CU_ASSERT(indexBucket[l][l] == 1);
        CU_ASSERT(indexChain[l][0] == 0);
    }
    */
    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);


    // Third Test - multiple same payloads, ascending order //

    /*  Input   orderRel | Output IndexChain |       Output IndexBucket
     *  (rowId, payload) |                   |
     *      (1, 0)       |      [0, 1]   <-  | [2, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (2, 0)       |      [0, 1]   <-  | [0, 2, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (3, 1)       |      [0, 1]   <-  | [0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (4, 1)       |      [0, 1]   <-  | [0, 0, 0, 2, 0, 0, 0, 0, 0, 0, ...]
     *      (5, 2)       |      [0, 1]   <-  | [0, 0, 0, 0, 2, 0, 0, 0, 0, 0, ...]
     *      (6, 2)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (7, 3)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (8, 3)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *      (9, 4)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *     (10, 4)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
     *        -          |        -          |                -
     */


    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = i/2;
    }
    rel.numTuples = 10;
    orderedRel.numTuples = rel.numTuples;

    RHJ_stage1(&rel, &orderedRel, pSum);

    /* === Stage 2 testing === */

    RHJ_stage2(&orderedRel, pSum, &indexBucket, &indexChain);
    /*
    for (int l = 0; l < 10; ++l) {
        for (int j = 0; j < 10 ; ++j) {
            if (j != l) {
                CU_ASSERT(indexBucket[l][j] == 0);
            }
        }
        if (l < 5) {
            CU_ASSERT(indexBucket[l][l] == 2);
            CU_ASSERT(indexChain[l][0] == 0);
            CU_ASSERT(indexChain[l][1] == 1);
        }
        else {
            CU_ASSERT(indexBucket[l][l] == 0);
        }

    }
    */
    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);


    // Fourth Test - multiple same payloads, descending order //

   /*  Input   orderRel | Output IndexChain |       Output IndexBucket
    *  (rowId, payload) |                   |
    *      (9, 0)       |      [0, 1]   <-  | [2, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
    *     (10, 0)       |      [0, 1]   <-  | [0, 2, 0, 0, 0, 0, 0, 0, 0, 0, ...]
    *      (7, 1)       |      [0, 1]   <-  | [0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ...]
    *      (8, 1)       |      [0, 1]   <-  | [0, 0, 0, 2, 0, 0, 0, 0, 0, 0, ...]
    *      (5, 2)       |      [0, 1]   <-  | [0, 0, 0, 0, 2, 0, 0, 0, 0, 0, ...]
    *      (6, 2)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
    *      (3, 3)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
    *      (4, 3)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
    *      (1, 4)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
    *      (2, 4)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...]
    *        -          |        -          |                -
    */


    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = (9 - i)/2;
    }
    rel.numTuples = 10;
    orderedRel.numTuples = rel.numTuples;

    RHJ_stage1(&rel, &orderedRel, pSum);

    /* === Stage 2 testing === */

    RHJ_stage2(&orderedRel, pSum, &indexBucket, &indexChain);
    /*
    for (int l = 0; l < 10; ++l) {
        for (int j = 0; j < 10 ; ++j) {
            if (j != l) {
                CU_ASSERT(indexBucket[l][j] == 0);
            }
        }
        if (l < 5) {
            CU_ASSERT(indexBucket[l][l] == 2);
            CU_ASSERT(indexChain[l][0] == 0);
            CU_ASSERT(indexChain[l][1] == 1);
        }
        else {
            CU_ASSERT(indexBucket[l][l] == 0);
        }

    }
    */
    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);

    // Fifth Test - all same payloads //

    /*  Input   orderRel | Output IndexChain |        Output IndexBucket
     *  (rowId, payload) |                   |
     *      (1, 7)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0,  0, 0, 0, ...]
     *      (2, 7)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0,  0, 0, 0, ...]
     *      (3, 7)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0,  0, 0, 0, ...]
     *      (4, 7)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0,  0, 0, 0, ...]
     *      (5, 7)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0,  0, 0, 0, ...]
     *      (6, 7)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0,  0, 0, 0, ...]
     *      (7, 7)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0,  0, 0, 0, ...]
     *      (8, 7)       | [0, 1, .., 9] <-  | [0, 0, 0, 0, 0, 0, 0, 10, 0, 0, ...]
     *      (9, 7)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0,  0, 0, 0, ...]
     *     (10, 7)       |       [ ]     <-  | [0, 0, 0, 0, 0, 0, 0,  0, 0, 0, ...]
     *        -          |        -          |                -
     */

    for (i = 0; i < 10; ++i) {
        rel.tuples[i].rowId = i + 1;
        rel.tuples[i].payload = 7;
    }
    rel.numTuples = 10;
    orderedRel.numTuples = rel.numTuples;

    RHJ_stage1(&rel, &orderedRel, pSum);

    /* === Stage 2 testing === */

    RHJ_stage2(&orderedRel, pSum, &indexBucket, &indexChain);

    for (int l = 0; l < 10; ++l) {
        for (int j = 0; j < 10 ; ++j) {
            CU_ASSERT(indexBucket[l][j] == 0);
        }
    }

    for (int k = 0; k < 10; ++k) {
        CU_ASSERT(indexChain[7][k] == k);
    }

    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);


    free(rel.tuples);
    free(orderedRel.tuples);
}

void testRHJ_stage3(void) {

    int index,i,j;

    Result *current;
    Relation smallStartRel, largeStartRel;   // These are input relations
    Relation smallRel, largeRel;    // These are orderedRels
    uint64_t smallPsum[H1BUCKETS], largePsum[H1BUCKETS];

    smallStartRel.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(smallStartRel.tuples, __FILE__, __func__, __LINE__);
    largeStartRel.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(largeStartRel.tuples, __FILE__, __func__, __LINE__);
    smallRel.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(smallRel.tuples, __FILE__, __func__, __LINE__);
    largeRel.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(largeRel.tuples, __FILE__, __func__, __LINE__);

    // Test 1 - all same payloads //
    // All rowIds should match //

    /*  Input   smallRel | Input   largeRel |  Output Result Tuples
     *  (rowId1, payload)| (rowId2, payload)|   (rowId1, rowId2)
     *      (1, 0)       |     (1, 0)       |      (1, 1)
     *      (2, 0)       |     (2, 0)       |      (1, 2)
     *      (3, 0)       |     (3, 0)       |      (1, 3)
     *      (4, 0)       |     (4, 0)       |      (1, 4)
     *      (5, 0)       |     (5, 0)       |      (1, 5)
     *      (6, 0)       |     (6, 0)       |      (1, 6)
     *      (7, 0)       |     (7, 0)       |      (1, 7)
     *      (8, 0)       |     (8, 0)       |      (1, 8)
     *      (9, 0)       |     (9, 0)       |      (1, 9)
     *     (10, 0)       |    (10, 0)       |      (1, 10)
     *        -          |       -          |      (2, 1)
     *        -          |       -          |      ......
     *        -          |       -          |     (10, 10)
     */

    for (i = 0; i < 10; ++i) {
        smallStartRel.tuples[i].rowId = i + 1;
        smallStartRel.tuples[i].payload = 0;
        largeStartRel.tuples[i].rowId = i + 1;
        largeStartRel.tuples[i].payload = 0;
    }
    smallStartRel.numTuples = 10;
    largeStartRel.numTuples = 10;
    smallRel.numTuples = 10;
    largeRel.numTuples = 10;


    RHJ_stage1(&smallStartRel, &smallRel, smallPsum);
    RHJ_stage1(&largeStartRel, &largeRel, largePsum);

    RHJ_stage2(&smallRel, smallPsum, &indexBucket, &indexChain);

    RHJ_stage3(res, &smallRel, &largeRel, smallPsum, largePsum, indexChain, indexBucket);
    index = 0;
    j = 0;
    current = res->first;
    while (index < 100) {
        for (i = 0; i < 10; ++i) {
            CU_ASSERT(current->buffer[index].rowId1 == 10 - i);   // rowIds[0] = ids of smallRel
            CU_ASSERT(current->buffer[index].rowId2 == j + 1);   // rowIds[1] = ids of largeRel
            index++;
        }
        j++;

    }

    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);

    resultRestart(res);

    // Test 2 //
    // Some rowIds will be matched //

    /*  Input   smallRel | Input   largeRel |  Output Result Tuples
     *  (rowId1, payload)| (rowId2, payload)|   (rowId1, rowId2)
     *      (1, 0)       |     (1, 0)       |      (1, 1)
     *      (2, 1)       |     (2, 1)       |      (2, 2)
     *      (3, 2)       |     (3, 2)       |      (3, 3)
     *      (4, 3)       |     (4, 3)       |      (4, 4)
     *      (5, 4)       |     (5, 4)       |      (5, 5)
     *      (6, 5)       |     (6, 5)       |      (6, 6)
     *      (7, 6)       |     (7, 6)       |      (7, 7)
     *      (8, 7)       |     (8, 7)       |      (8, 8)
     *      (9, 8)       |     (9, 8)       |      (9, 9)
     *     (10, 9)       |    (10, 9)       |     (10, 10)
     *        -          |       -          |        -
     */

    for (i = 0; i < 10; ++i) {
        smallStartRel.tuples[i].rowId = i + 1;
        smallStartRel.tuples[i].payload = i;
        largeStartRel.tuples[i].rowId = i + 1;
        largeStartRel.tuples[i].payload = i;
    }
    smallStartRel.numTuples = 10;
    largeStartRel.numTuples = 10;
    smallRel.numTuples = 10;
    largeRel.numTuples = 10;


    RHJ_stage1(&smallStartRel, &smallRel, smallPsum);
    RHJ_stage1(&largeStartRel, &largeRel, largePsum);

    RHJ_stage2(&smallRel, smallPsum, &indexBucket, &indexChain);

    RHJ_stage3(res, &smallRel, &largeRel, smallPsum, largePsum, indexChain, indexBucket);
    index = 0;
    current = res->first;
    while (index < 10) {
        CU_ASSERT(current->buffer[index].rowId1 == index + 1);   // rowIds[0] = ids of smallRel
        CU_ASSERT(current->buffer[index].rowId2 == index + 1);   // rowIds[1] = ids of largeRel
        index++;
    }

    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);

    resultRestart(res);

    // Test 3 //
    // No rowIds will be matched //

    /*  Input   smallRel | Input   largeRel |  Output Result Tuples
    *  (rowId1, payload)| (rowId2, payload)|   (rowId1, rowId2)
    *      (1, 0)       |     (1, 10)       |        -
    *      (2, 1)       |     (2, 11)       |        -
    *      (3, 2)       |     (3, 12)       |        -
    *      (4, 3)       |     (4, 13)       |        -
    *      (5, 4)       |     (5, 14)       |        -
    *      (6, 5)       |     (6, 15)       |        -
    *      (7, 6)       |     (7, 16)       |        -
    *      (8, 7)       |     (8, 17)       |        -
    *      (9, 8)       |     (9, 18)       |        -
    *     (10, 9)       |    (10, 19)       |        -
    *        -          |       -           |        -
    */

    for (i = 0; i < 10; ++i) {
        smallStartRel.tuples[i].rowId = i + 1;
        smallStartRel.tuples[i].payload = i;
        largeStartRel.tuples[i].rowId = i + 1;
        largeStartRel.tuples[i].payload = i + 10;
    }
    smallStartRel.numTuples = 10;
    largeStartRel.numTuples = 10;
    smallRel.numTuples = 10;
    largeRel.numTuples = 10;


    RHJ_stage1(&smallStartRel, &smallRel, smallPsum);
    RHJ_stage1(&largeStartRel, &largeRel, largePsum);

    RHJ_stage2(&smallRel, smallPsum, &indexBucket, &indexChain);

    RHJ_stage3(res, &smallRel, &largeRel, smallPsum, largePsum, indexChain, indexBucket);

    CU_ASSERT(res->last->numOfTuples == 0);

    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);

    resultRestart(res);

    // Test 4 //
    // Relations of different size. Some rowIds will be matched //

    /*  Input   smallRel | Input   largeRel |  Output Result Tuples
     *  (rowId1, payload)| (rowId2, payload)|   (rowId1, rowId2)
     *      (1, 0)       |     (1, 1)       |      (2, 1)
     *      (2, 1)       |     (2, 2)       |      (3, 2)
     *      (3, 2)       |     (3, 3)       |      (4, 3)
     *      (4, 3)       |     (4, 4)       |      (5, 4)
     *      (5, 4)       |     (5, 5)       |        -
     *        -          |     (6, 6)       |        -
     *        -          |     (7, 7)       |        -
     *        -          |     (8, 8)       |        -
     *        -          |     (9, 9)       |        -
     *        -          |    (10, 10)      |        -
     *        -          |       -          |        -
     */

    for (i = 0; i < 10; ++i) {
        if (i < 5) {
            smallStartRel.tuples[i].rowId = i + 1;
            smallStartRel.tuples[i].payload = i;
        }
        largeStartRel.tuples[i].rowId = i + 1;
        largeStartRel.tuples[i].payload = i + 1;
    }
    smallStartRel.numTuples = 5;
    largeStartRel.numTuples = 10;
    smallRel.numTuples = 5;
    largeRel.numTuples = 10;


    RHJ_stage1(&smallStartRel, &smallRel, smallPsum);
    RHJ_stage1(&largeStartRel, &largeRel, largePsum);

    RHJ_stage2(&smallRel, smallPsum, &indexBucket, &indexChain);

    RHJ_stage3(res, &smallRel, &largeRel, smallPsum, largePsum, indexChain, indexBucket);

    index = 0;
    current = res->first;
    while (index < 4) {
        CU_ASSERT(current->buffer[index].rowId1 == index + 2);   // rowIds[0] = ids of smallRel
        CU_ASSERT(current->buffer[index].rowId2 == index + 1);   // rowIds[1] = ids of largeRel
        index++;
    }

    deleteInnerChain(indexChain);
    deleteIndex(indexBucket, indexChain);

    resultRestart(res);

    free(smallStartRel.tuples);
    free(smallRel.tuples);
    free(largeStartRel.tuples);
    free(largeRel.tuples);
}


void testRadixHashJoin(void) {

    // Tests are the same with testRHJ_stage3

    Relation rel1, rel2;
    Result *current;

    int i, j, index;

    rel1.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(rel1.tuples, __FILE__, __func__, __LINE__);
    rel2.tuples = malloc(10*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK(rel2.tuples, __FILE__, __func__, __LINE__);

    // Test 1 - all same payloads //
    // All rowIds should match //

    for (i = 0; i < 10; ++i) {
        rel1.tuples[i].rowId = i + 1;
        rel1.tuples[i].payload = 0;
        rel2.tuples[i].rowId = i + 1;
        rel2.tuples[i].payload = 0;
    }
    rel1.numTuples = 10;
    rel2.numTuples = 10;

    res = RadixHashJoin(&rel1, &rel2);

    index = 0;
    j = 0;
    current = res->first;
    while (index < 100) {
        for (i = 0; i < 10; ++i) {
            CU_ASSERT(current->buffer[index].rowId1 == 10 - i);   // rowIds[0] = ids of smallRel
            CU_ASSERT(current->buffer[index].rowId2 == j + 1);   // rowIds[1] = ids of largeRel
            index++;
        }
        j++;

    }

    resultRestart(res);

    // Test 2 //
    // Some rowIds will be matched //

    for (i = 0; i < 10; ++i) {
        rel1.tuples[i].rowId = i + 1;
        rel1.tuples[i].payload = i;
        rel2.tuples[i].rowId = i + 1;
        rel2.tuples[i].payload = i;
    }
    rel1.numTuples = 10;
    rel2.numTuples = 10;

    res = RadixHashJoin(&rel1, &rel2);

    index = 0;
    current = res->first;
    while (index < 10) {
        CU_ASSERT(current->buffer[index].rowId1 == index + 1);   // rowIds[0] = ids of smallRel
        CU_ASSERT(current->buffer[index].rowId2 == index + 1);   // rowIds[1] = ids of largeRel
        index++;
    }

    resultRestart(res);

    // Test 3 //
    // No rowIds will be matched //

    for (i = 0; i < 10; ++i) {
        rel1.tuples[i].rowId = i + 1;
        rel1.tuples[i].payload = i;
        rel2.tuples[i].rowId = i + 1;
        rel2.tuples[i].payload = i + 10;
    }
    rel1.numTuples = 10;
    rel2.numTuples = 10;

    res = RadixHashJoin(&rel1, &rel2);

    CU_ASSERT(res->last->numOfTuples == 0);

    resultRestart(res);

    // Test 4 //
    // Relations of different size. Some rowIds will be matched //

    for (i = 0; i < 10; ++i) {
        if (i < 5) {
            rel1.tuples[i].rowId = i + 1;
            rel1.tuples[i].payload = i;
        }
        rel2.tuples[i].rowId = i + 1;
        rel2.tuples[i].payload = i + 1;
    }
    rel1.numTuples = 5;
    rel2.numTuples = 10;

    res = RadixHashJoin(&rel1, &rel2);

    index = 0;
    current = res->first;
    while (index < 4) {
        CU_ASSERT(current->buffer[index].rowId1 == index + 2);   // rowIds[0] = ids of smallRel
        CU_ASSERT(current->buffer[index].rowId2 == index + 1);   // rowIds[1] = ids of largeRel
        index++;
    }

    resultRestart(res);

    free(rel1.tuples);
    free(rel2.tuples);
}