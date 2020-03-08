#ifndef ERGASIA2_ALL_SQL_QUERYPARSER_H
#define ERGASIA2_ALL_SQL_QUERYPARSER_H

#include "../include/relations.h"
#include "../include/result.h"

void mainQueryParser(char *, ArrayHead *);

ArrayList ** SQL_arrayIndexing(char *, ArrayHead *, int);

void SQL_runFilterPredicate(ArrayList *, int, int, char, uint64_t, IntermediateResultHead *);

void SQL_runConjuctionPredicate(ArrayList *, int , int , ArrayList *, int, int, IntermediateResultHead *);

void ProjectionJob(void *);

void SQL_runProjections(char *, ArrayList **, IntermediateResultHead *);


#endif