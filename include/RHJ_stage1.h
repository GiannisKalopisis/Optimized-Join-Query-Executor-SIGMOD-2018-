//
// Created by kalo-pc on 26/10/2018.
//

#ifndef RHJ_STAGE1_H
#define RHJ_STAGE1_H

#include <stdint.h>

#include "relations.h"

void HistogramJob(void *);
void PartitionJob(void *);
void RHJ_stage1(Relation *, Relation *, uint64_t *);

#endif
