#ifndef RESULT_H
#define RESULT_H

#include <stdint.h>


#define BUFFERSIZE 1024*128 // 128 KB


typedef struct IntermediateResultHead {
    struct IntermediateResult *first;
    struct IntermediateResult *last;
} IntermediateResultHead;

typedef struct IntermediateResult {
    int relationId;
    int numRowIds;
    uint64_t *rowIds;
    int numJoinedRels;
    struct IntermediateResult **joinedRels;
    struct IntermediateResult *next;
} IntermediateResult;

void intermediateResultInit(IntermediateResultHead **);

void intermediateResultCreateNewNode(IntermediateResultHead *, int , int );

void intermediateResultAddJoinedRel(IntermediateResult *, IntermediateResult * );

int intermediateResultIsContained(IntermediateResultHead *, int );

IntermediateResult * intermediateResultGetNode(IntermediateResultHead *, int );

void intermediateResultUpdateSize(IntermediateResultHead *, int , int );

void intermediateResultUpdateOtherNodes(IntermediateResultHead *, int , int );

void intermediateResultPrintAll(IntermediateResultHead *);

void intermediateResultDestroy(IntermediateResultHead *);


typedef struct ResultTuple {
    uint64_t rowId1;
    uint64_t rowId2;
} ResultTuple;

typedef struct ResultHead {
    struct Result *first;
    struct Result *last;
    int maxTuples;
} ResultHead;

typedef struct Result {
    int numOfTuples;
    ResultTuple *buffer;
    struct Result *next;
} Result;

void resultInit(ResultHead **);

Result * resultCreateNewNode();

uint32_t resultIsLastNodeFull(ResultHead *);

void resultAppend(ResultHead *, uint64_t, uint64_t);

int resultGetTotalTuples(ResultHead *);

void resultPrintAll(ResultHead *);

void resultRestart(ResultHead *);

void resultDestroy(ResultHead *);


#endif
