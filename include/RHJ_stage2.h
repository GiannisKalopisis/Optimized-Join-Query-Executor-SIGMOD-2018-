//
// Created by kalo-pc on 26/10/2018.
//

#ifndef RHJ_STAGE2_H
#define RHJ_STAGE2_H


#include <stdint.h>

#include "relations.h"

void RHJ_stage2(Relation *, uint64_t *, uint64_t ***, uint64_t ***);

void chainCreation(Relation *, uint64_t *, uint64_t ***);

//void InitializeBucketJob(void *);

void initializeBuckets(uint64_t **);

void InitializeIndexJob(void *);

void initializationIndex(Relation *, uint64_t *, uint64_t **, uint64_t **);

void deleteInnerChain(uint64_t **);

void deleteIndex(uint64_t **, uint64_t **);



#endif
