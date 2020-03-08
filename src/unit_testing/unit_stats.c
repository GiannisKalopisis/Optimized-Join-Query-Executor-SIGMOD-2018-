//
// Created by kalo-pc on 5/1/2019.
//

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "../../include/unit_stats.h"
#include "../../include/general_functions.h"
#include "../../include/stats.h"
#include "../../include/SQL_QueryParser.h"


void testStatsInitInitialize(void){


}


void testStatsInit(void){

    char *relations = strtok(oldQuery, "|");
    char *predicates = strtok(NULL, "|");
    char *projections = strtok(NULL, "|\0\n");

    int numOfRelations = getSpaces(relations);
    numOfRelations++;

    ArrayList **indexedListNodes = NULL;
    indexedListNodes = SQL_arrayIndexing(relations, headRelationList, numOfRelations);

    uint64_t ***stats = NULL;
    statsInitInitialize(indexedListNodes, numOfRelations, &stats);

}


void testStatsInitialize(void){

}


void testStatsCalculatePred(void){

}
