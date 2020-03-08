//
// Created by dodor on 04/01/19.
//

#ifndef RADIX_HASH_JOIN_STATS_H
#define RADIX_HASH_JOIN_STATS_H

#include <stdint.h>

#include "../include/relations.h"
#include "../include/predicateParser.h"


void statsInitInitialize(ArrayList **arrayList, int numOfRelations, uint64_t ****stats);

void statsInit(ArrayList **arrayList, int numOfRelations, uint64_t ****stats);

void statsInitialize(ArrayList **arrayList, int numOfRelations, uint64_t ***stats);

void statsCalculatePred(ArrayList **arrayList, predicateList *curPred, uint64_t ***curStats, int numOfRelations);

void statsDestroy(ArrayList **arrayList, int numOfRelations, uint64_t ***stats);

int statsCalculatePredForJoinEnumeration(ArrayList **arrayList, int *R_array, predicateListHead *predicateHead, uint64_t ***curStats, int sizeOfArray, int numOfRelations);

#endif //RADIX_HASH_JOIN_STATS_H
