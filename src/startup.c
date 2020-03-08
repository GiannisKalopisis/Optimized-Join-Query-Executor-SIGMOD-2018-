#include <stdlib.h>
#include <stdint.h>

#include "../include/RHJ_stage2.h"
#include "../include/hash_functions.h"
#include "../include/job_scheduler.h"
#include "../include/startup.h"
#include "../include/error_check.h"
#include "../include/result.h"

extern uint64_t **histogram;
extern uint64_t **positions;
extern uint64_t *pSumR;
extern uint64_t *pSumS;
extern uint64_t **indexBucket;
extern uint64_t **indexChain;
extern ResultHead **results;
extern ResultHead *res;

// Memory allocation and deallocation for SMALL and frequently used structures of constant size

void init() {

    // Histograms & Positions ( rhj_stage1 )

    histogram = malloc(NUM_THREADS * sizeof(uint64_t*));
    NULL_POINTER_ERROR_CHECK(histogram, __FILE__, __func__, __LINE__);

    positions = malloc(NUM_THREADS * sizeof(uint64_t));
    NULL_POINTER_ERROR_CHECK(positions, __FILE__, __func__, __LINE__);

    // main result ( rhj )

    resultInit(&res);

    // Job Scheduler

    jsInit(NUM_THREADS);
}

void destroy() {

    // Job Scheduler

    jsStop();

    jsDestroy();

    // main result ( rhj )

    resultDestroy(res);

    // Histograms & Positions  ( rhj_stage1 )

    free(histogram);
    free(positions);

}