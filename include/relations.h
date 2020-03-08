
#ifndef RELATIONS_H
#define RELATIONS_H

#include <stdint.h>

// Defined indexes for statistics array
#define MIN_VAL 0  // Minimum value (l)
#define MAX_VAL 1  // Maximum value (u)
#define NUM_VAL 2  // Num of values (f)
#define DIS_VAL 3  // Num of distinct values (d)

// Max distinct values to calculate (and it's a prime number)
// We should use this technique only for our given datasets
#define N 49999991

typedef struct Tuple {
    uint64_t rowId;
    uint64_t payload;
} Tuple;

typedef struct Relation {
    Tuple *tuples;
    uint64_t numTuples;
} Relation;

typedef struct ArrayList {
    uint64_t numOfRows;
    uint64_t numOfColumns;
    uint64_t **arrayOfInts;
    uint64_t **statistics;
    char ** distinctValuesArr;
    struct ArrayList *next;
} ArrayList;

typedef struct ArrayHead {
    uint64_t numOfNodes;
    ArrayList *first;
    ArrayList *last;
} ArrayHead;


void arrayHeadInit(ArrayHead **);

void arrayListInit(ArrayHead *, uint64_t, uint64_t);

void arrayListInsert(ArrayHead *, char *);

ArrayList * arrayListIndex(ArrayHead *, uint64_t);

void arrayListDestroy(ArrayHead *);

void rInit(Relation **, uint64_t);

void rNewInsert(Relation *, uint64_t *, int, uint64_t *);

void rInsert(Relation *, uint64_t **, int, int);

uint64_t rGetPayloadOfIndex(Relation *, uint64_t);

void rDestroy(Relation *);


#endif