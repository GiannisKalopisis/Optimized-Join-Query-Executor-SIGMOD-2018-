//
// Created by kalo-pc on 23/11/2018.
//

#ifndef RADIX_HASH_JOIN_PROJECT1_MASTER_PREDICATEPARSER_H
#define RADIX_HASH_JOIN_PROJECT1_MASTER_PREDICATEPARSER_H

#include <stdint.h>

#define FILTER_EQ    0   // Equality filter
#define FILTER_LT_GT 1   // Less than / Greater than filter
#define FILTER_SA    2   // Same array, different columns filter
#define JOIN_DA      3   // Join of different arrays
#define JOIN_SA      4   // Join of same array and column

/*
 *  isFilter = 0 --> no filter
 *  isFilter = 1 --> > filter
 *  isFilter = 2 --> < filter
 *  isFilter = 3 --> = filter
 *  isFilter = 0 --> same array filter
 *  isFilter = 0 --> exists intermediate result filter
 */

typedef struct predicateList {

    int isFilter;   // 0 == no, 1 == ">", 2 == "<", 3 == "=".

    uint64_t filterValue;

    char *predicate;

    // Larger number equals higher priority, -1 means done
    int priority;   // 0...n == joins , n + 1 == filters

    int predType;

    int array1;
    int columnOfArray1;
    int Fc_1;

    int array2;
    int columnOfArray2;
    int Fc_2;

    struct predicateList *next;

} predicateList;


typedef struct predicateListHead {

    struct predicateList *first;
    struct predicateList *last;
    int numOfPredicates;

} predicateListHead;


typedef struct tempPriorityStruct {

    int array1;
    int colarray1;
    int array2;
    int colarray2;

} tempPriorityStruct;


void predicateListHeadInit(predicateListHead **);

void predicateListCreation(predicateListHead *, char *);

void predicateListInsert(predicateList *, predicateListHead *);

void predicateListNodeCreation(predicateListHead *, char *);

void DestroyPredicateList(predicateListHead *);

void predicateListCalculatePriorities(predicateListHead *, int *, int);

void updatePriority(predicateListHead *, int, int);

predicateList *getHighestPriorityPredicate(predicateListHead *);

#endif //RADIX_HASH_JOIN_PROJECT1_MASTER_PREDICATEPARSER_H
