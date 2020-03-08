#ifndef RHJ_STAGE3_H
#define RHJ_STAGE3_H

#include <stdint.h>

#include "relations.h"
#include "result.h"

void JoinJob(void *);
void RHJ_stage3(ResultHead *, Relation *, Relation *, uint64_t *, uint64_t *, uint64_t **, uint64_t **);


#endif