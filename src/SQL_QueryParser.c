//
// Created by dodor on 22/11/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/SQL_QueryParser.h"
#include "../include/relations.h"
#include "../include/error_check.h"
#include "../include/general_functions.h"
#include "../include/result.h"
#include "../include/RHJ.h"
#include "../include/predicateParser.h"
#include "../include/job_scheduler.h"
#include "../include/stats.h"
#include "../include/joinEnumeration.h"


#define HARNESS 1

extern JobScheduler *const jobScheduler;

// Globals for ProjectionJob
IntermediateResultHead *intResHead;
ArrayList **indexListNodes;
uint64_t *counters;
int *projectionArray[2];


void mainQueryParser(char *oldQuery, ArrayHead *headRelationList){

    char *query = (char *) malloc((strlen(oldQuery) + 1) * sizeof(char));
    strcpy(query, oldQuery);

    char *relations = strtok(oldQuery, "|");
    char *predicates = strtok(NULL, "|");
    char *projections = strtok(NULL, "|\0\n");

    int numOfRelations = getSpaces(relations);
    numOfRelations++;

    ArrayList **indexedListNodes = NULL;
    indexedListNodes = SQL_arrayIndexing(relations, headRelationList, numOfRelations);

    /*initializing statistics for all and keep the to reinitialize them*/
    uint64_t ***initStats = NULL;
    statsInitInitialize(indexedListNodes, numOfRelations, &initStats);

    /*creating intermediate result*/
    IntermediateResultHead *iResHead;
    intermediateResultInit(&iResHead);

    /*pointing to predicates*/
    predicateListHead *predicateHead;
    predicateListHeadInit(&predicateHead);
    predicateListCreation(predicateHead, predicates);

    /*statistics for filters*/
    uint64_t ***afterFilterStats;
    statsInitInitialize(indexedListNodes, numOfRelations, &afterFilterStats);

    predicateList *curPredForFilter = predicateHead->first;

    //int count = 0;

    while (curPredForFilter != NULL) {
        if (curPredForFilter->predType != JOIN_DA) {
            statsCalculatePred(indexedListNodes, curPredForFilter, afterFilterStats, numOfRelations);
            //count++;
        }

        curPredForFilter = curPredForFilter->next;
    }

    //int nonFilterRelations = numOfRelations - count;

    /*R_array is array of priority for joins*/
    int *R_array = (int *)malloc((predicateHead->numOfPredicates*2) * sizeof(int));
    NULL_POINTER_ERROR_CHECK(R_array, __FILE__, __func__, __LINE__);
    int *R_array2 = (int *)malloc((predicateHead->numOfPredicates*2) * sizeof(int));
    NULL_POINTER_ERROR_CHECK(R_array2, __FILE__, __func__, __LINE__);



    for (int l = 0; l < (predicateHead->numOfPredicates*2); ++l) {
        R_array[l] = -1;
        R_array2[l] = -1;
    }

    //nonFilterRelations = 0;
    int temp = 0;

    //printf("nonFilterRelations = %d\n", nonFilterRelations);

    curPredForFilter = predicateHead->first;
    while (curPredForFilter != NULL) {
        //printf("...%d\n", curPredForFilter->predType);
        //printf(" array1 = %d, array2 = %d, col1 = %d, col2 = %d\n", curPredForFilter->array1, curPredForFilter->array2, curPredForFilter->columnOfArray1, curPredForFilter->columnOfArray2);
        if (curPredForFilter->predType == JOIN_DA) {
            for (int i = 0; i < (predicateHead->numOfPredicates*2); ++i) {
                if (R_array[i] == curPredForFilter->array1) {
                    break;
                }
                else if (R_array[i] == -1) {
                    R_array[i] = curPredForFilter->array1;
                    temp++;
                    break;
                }
            }
            for (int i = 0; i < (predicateHead->numOfPredicates*2); ++i) {
                if (R_array[i] == curPredForFilter->array2) {
                    break;
                }
                else if (R_array[i] == -1) {
                    R_array[i] = curPredForFilter->array2;
                    temp++;
                    break;
                }
            }
        }
        curPredForFilter = curPredForFilter->next;
    }
    //printf("temp = %d\n", temp);

    for (int m = 0; m < temp; ++m) {
        R_array2[m] = R_array[temp-m-1];
    }

    R_array = realloc(R_array, temp*sizeof(int));
    NULL_POINTER_ERROR_CHECK(R_array, __FILE__, __func__, __LINE__);


    //printf("%s\n", query);
    for (int i = 0; i < temp; ++i) {
        //printf("R_array[%d] = %d\n", i, R_array[i]);
        //printf("R_array2[%d] = %d\n", i, R_array2[i]);
    }


    //printf("nonFilterRelations = %d\n", nonFilterRelations);
    joinEnumerationFunction(indexedListNodes, predicateHead, temp, R_array, afterFilterStats, numOfRelations);
    //uint64_t ***afterFilterStats = statsCalculatePred(indexedListNodes, predicateHead->first, stats);

    //printf("Join Enumeration done\n");

    predicateListCalculatePriorities(predicateHead, R_array, temp);


    for (int k = 0; k < temp; ++k) {
        //printf("R_array[%d] = %d\n", k, R_array[k]);
        //printf("R_array2[%d] = %d\n", k, R_array2[k]);
    }
    //printf("\n");

    statsDestroy(indexedListNodes, numOfRelations, afterFilterStats);
    //predicateListCalculatePriorities(predicateHead);

    predicateList *curPredicate = NULL;
    while ( (curPredicate = getHighestPriorityPredicate(predicateHead)) != NULL ) {
        //printf("%s\n", curPredicate->predicate);
        if (curPredicate->isFilter) {
            switch (curPredicate->isFilter) {
                case 1: // >
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1,
                                           curPredicate->columnOfArray1, '>', curPredicate->filterValue, iResHead);
                    break;
                case 2: // <
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1,
                                           curPredicate->columnOfArray1, '<', curPredicate->filterValue, iResHead);
                    break;
                case 3: // =
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1,
                                           curPredicate->columnOfArray1, '=', curPredicate->filterValue, iResHead);
                    break;
            }
            //updatePriority(predicateHead, curPredicate->array1, -1);
        }
        // Conjunction query
        else {
            SQL_runConjuctionPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1,
                                       indexedListNodes[curPredicate->array2], curPredicate->array2, curPredicate->columnOfArray2,
                                       iResHead);
            //updatePriority(predicateHead, curPredicate->array1, curPredicate->array2);
        }
        //intermediateResultPrintAll(iResHead);
    }

    for (int i = 0; i < numOfRelations; ++i) {
        for (int j = 0; j < indexedListNodes[i]->numOfColumns; ++j) {
            indexedListNodes[i]->statistics[j][MIN_VAL] = initStats[i][j][MIN_VAL];
            indexedListNodes[i]->statistics[j][MAX_VAL] = initStats[i][j][MAX_VAL];
            indexedListNodes[i]->statistics[j][NUM_VAL] = initStats[i][j][NUM_VAL];
            indexedListNodes[i]->statistics[j][DIS_VAL] = initStats[i][j][DIS_VAL];
        }
    }

    SQL_runProjections(projections, indexedListNodes, iResHead);

    statsDestroy(indexedListNodes, numOfRelations, initStats);

    free(R_array);
    free(R_array2);


    free(indexedListNodes);
    intermediateResultDestroy(iResHead);

    free(query);
    DestroyPredicateList(predicateHead);
}


ArrayList ** SQL_arrayIndexing(char *fromField, ArrayHead *headRelationList, int numOfRelations) {

    uint64_t curIndex;
    char *tmp = NULL, *token = NULL;

    tmp = (char *) malloc((strlen(fromField) + 1) * sizeof(char));
    NULL_POINTER_ERROR_CHECK(tmp, __FILE__, __func__, __LINE__);

    // Allocate memory for that many relation pointers

    ArrayList **indexedListNodes = malloc(numOfRelations * sizeof(ArrayList *));
    NULL_POINTER_ERROR_CHECK(indexedListNodes, __FILE__, __func__, __LINE__);

    // Do the indexing

    strcpy(tmp, fromField);
    token = strtok(tmp," ");
    for (int i = 0; i < numOfRelations; ++i) {

        // Could check for invalid FROM field tokens here
        // check return value of atoi maybe? if that index doesn't exist? etc.

        curIndex = atoi(token);

        indexedListNodes[i] = arrayListIndex(headRelationList, curIndex);

        token = strtok(NULL," ");
    }

    if (tmp != NULL) free(tmp);

    return indexedListNodes;
}


void SQL_runFilterPredicate(ArrayList *Relation, int relationId, int column,
                            char oper, uint64_t value, IntermediateResultHead * iResHead) {

    IntermediateResult *iRes;
    uint64_t curRowId;
    int passFilter = 0;

    // Using a priority system for the predicates, all filter predicates will be run
    // before any conjunction predicates. With that said, no management for joined
    // relation results is needed.

    // If relation has intermediate rowId results
    if (intermediateResultIsContained(iResHead,relationId)) {

        iRes = intermediateResultGetNode(iResHead, relationId);

        for (int j = 0; j < iRes->numRowIds; ++j) {
            curRowId = iRes->rowIds[j];
            switch (oper) {
                case '>':
                    if (Relation->arrayOfInts[column][curRowId] > value) {
                        iRes->rowIds[passFilter] = curRowId;
                        passFilter++;
                    }
                    break;
                case '<':
                    if (Relation->arrayOfInts[column][curRowId] < value) {
                        iRes->rowIds[passFilter] = curRowId;
                        passFilter++;
                    }
                    break;
                case '=':
                    if (Relation->arrayOfInts[column][curRowId] == value) {
                        iRes->rowIds[passFilter] = curRowId;
                        passFilter++;
                    }
                    break;
            }
        }
    }
    // First appearance of this relation so far
    else {

        intermediateResultCreateNewNode(iResHead, relationId, Relation->numOfRows);
        iRes = intermediateResultGetNode(iResHead, relationId);

        for (int j = 0; j < Relation->numOfRows; ++j) {
            switch (oper) {
                case '>':
                    if (Relation->arrayOfInts[column][j] > value) {
                        iRes->rowIds[passFilter] = j;
                        passFilter++;
                    }
                    break;
                case '<':
                    if (Relation->arrayOfInts[column][j] < value) {
                        iRes->rowIds[passFilter] = j;
                        passFilter++;
                    }
                    break;
                case '=':
                    if (Relation->arrayOfInts[column][j] == value) {
                        iRes->rowIds[passFilter] = j;
                        passFilter++;
                    }
                    break;
            }
        }
    }

    // Update the rowIds array size in updated intermediate results
    intermediateResultUpdateSize(iResHead, relationId, passFilter);
}


// if Relation1 == Relation2 then do not join, just scan Relation1 (== Relation2) and get equal valued columns.

void SQL_runConjuctionPredicate(ArrayList *Relation1, int relationId1, int column1,
                                ArrayList *Relation2, int relationId2, int column2,
                                IntermediateResultHead * iResHead) {

    IntermediateResult *iRes1, *iRes2;
    uint64_t curRowId, newResultsIndex;
    // In this case just scan the Relation and get rowIds of equal valued columns.
    if (relationId1 == relationId2) {

        // If relation has intermediate rowId results
        if (intermediateResultIsContained(iResHead,relationId1)) {

            iRes1 = intermediateResultGetNode(iResHead, relationId1);

            newResultsIndex = 0;
            for (int i = 0; i < iRes1->numRowIds; ++i) {
                // Check only rowIds inside past results
                curRowId = iRes1->rowIds[i];
                if (Relation1->arrayOfInts[column1][curRowId] == Relation1->arrayOfInts[column2][curRowId]) {
                    // Update the joined relations if there are any
                    for (int j = 0; j < iRes1->numJoinedRels; ++j) {
                        iRes1->joinedRels[j]->rowIds[newResultsIndex] = iRes1->joinedRels[j]->rowIds[i];
                    }
                    // Add this rowId to updated intermediate results
                    iRes1->rowIds[newResultsIndex] = curRowId;
                    newResultsIndex++;
                }
            }
            // Update the rowIds array size in updated intermediate results
            intermediateResultUpdateSize(iResHead, relationId1, newResultsIndex);

            // Update the size of the joined relations if there are any
            for (int j = 0; j < iRes1->numJoinedRels; ++j) {
                intermediateResultUpdateSize(iResHead, iRes1->joinedRels[j]->relationId, newResultsIndex);
            }

        }
        // First appearance of this relation so far
        else {

            intermediateResultCreateNewNode(iResHead, relationId1, Relation1->numOfRows);
            iRes1 = intermediateResultGetNode(iResHead, relationId1);

            newResultsIndex = 0;
            for (uint64_t i = 0; i < Relation1->numOfRows; ++i) {
                if (Relation1->arrayOfInts[column1][i] == Relation1->arrayOfInts[column2][i]) {
                    // Add this rowId to intermediate results
                    iRes1->rowIds[newResultsIndex] = i;
                    newResultsIndex++;
                }
            }
            // Update the rowIds array size in updated intermediate results
            intermediateResultUpdateSize(iResHead, relationId1, newResultsIndex);
        }
    }
    // 2 different Relations, join the columns with radix hash join
    else {

        Relation *relation1, *relation2;
        int numTuplesRel1, numTuplesRel2;
        int isNewRes1 = 0, isNewRes2 = 0;
        // First check if relations have intermediate rowId results

        // First Relation
        if (intermediateResultIsContained(iResHead, relationId1)) {

            iRes1 = intermediateResultGetNode(iResHead, relationId1);

            // Check if iRes1->numRowIds == 0, if yes then end predicate function AFTER adding empty
            // intermediate results for both relationIds
            if (iRes1->numRowIds == 0) {
                // No matched rowIds will be returned if no RowIds are in intermediate results for this relation

                // Update the size of the joined relations if there are any
                for (int j = 0; j < iRes1->numJoinedRels; ++j) {
                    intermediateResultUpdateSize(iResHead, iRes1->joinedRels[j]->relationId, 0);
                }

                // If relation 2 has no intermediate results yet, create a node with numRowIds == 0
                if (!intermediateResultIsContained(iResHead, relationId2)) {
                    intermediateResultCreateNewNode(iResHead, relationId2, 0);
                }

                iRes2 = intermediateResultGetNode(iResHead, relationId2);

                // Update the size of the joined relations if there are any
                for (int j = 0; j < iRes2->numJoinedRels; ++j) {
                    intermediateResultUpdateSize(iResHead, iRes2->joinedRels[j]->relationId, 0);
                }

                // Update intermediate results for relation 2
                intermediateResultUpdateSize(iResHead, relationId2, 0);
                return;
            }

            rInit(&relation1, iRes1->numRowIds);
            rNewInsert(relation1, iRes1->rowIds, iRes1->numRowIds, Relation1->arrayOfInts[column1]);

            numTuplesRel1 = iRes1->numRowIds;
        }
        // First occurance of relationId1, create new intermediate results node
        else {
            isNewRes1 = 1;

            intermediateResultCreateNewNode(iResHead, relationId1, Relation1->numOfRows);
            iRes1 = intermediateResultGetNode(iResHead, relationId1);

            rInit(&relation1, Relation1->numOfRows);
            rNewInsert(relation1, NULL, Relation1->numOfRows, Relation1->arrayOfInts[column1]);

            numTuplesRel1 = Relation1->numOfRows;
        }

        // Second Relation, same code as first
        if (intermediateResultIsContained(iResHead, relationId2)) {

            iRes2 = intermediateResultGetNode(iResHead, relationId2);

            // Check if iRes2->numRowIds == 0, if yes then end predicate function AFTER adding empty
            // intermediate results for both relationIds
            if (iRes2->numRowIds == 0) {
                // No matched rowIds will be returned if no RowIds are in intermediate results for this relation

                // Update the size of the joined relations if there are any
                for (int j = 0; j < iRes2->numJoinedRels; ++j) {
                    intermediateResultUpdateSize(iResHead, iRes2->joinedRels[j]->relationId, 0);
                }

                iRes1 = intermediateResultGetNode(iResHead, relationId1);
                // Update intermediate results for relation 1 in case they aren't already "marked" empty

                // Update the size of the joined relations if there are any
                for (int j = 0; j < iRes1->numJoinedRels; ++j) {
                    intermediateResultUpdateSize(iResHead, iRes1->joinedRels[j]->relationId, 0);
                }

                intermediateResultUpdateSize(iResHead, relationId1, 0);

                // Deallocation of memory from relation 1 code
                rDestroy(relation1);

                return;
            }

            rInit(&relation2, iRes2->numRowIds);
            rNewInsert(relation2, iRes2->rowIds, iRes2->numRowIds, Relation2->arrayOfInts[column2]);

            numTuplesRel2 = iRes2->numRowIds;
        }
        // First occurance of relationId2, create new intermediate results node
        else {
            isNewRes2 = 1;

            intermediateResultCreateNewNode(iResHead, relationId2, Relation2->numOfRows);
            iRes2 = intermediateResultGetNode(iResHead, relationId2);

            rInit(&relation2, Relation2->numOfRows);
            rNewInsert(relation2, NULL, Relation2->numOfRows, Relation2->arrayOfInts[column2]);

            numTuplesRel2 = Relation2->numOfRows;
        }

        int isSameJoin = 0;
        for (int l = 0; l < iRes1->numJoinedRels; ++l) {
            if (iRes1->joinedRels[l]->relationId == relationId2) {
                isSameJoin = 1;
                break;
            }
        }

        IntermediateResult *curiRes;

        // If both relations already in same joined intermediate results
        // and if join columns are the same as the arguments of this function
        // then just scan the intermediate results arrays
        if (isSameJoin) {
            newResultsIndex = 0;
            uint64_t curRowId2;
            for (int i = 0; i < iRes1->numRowIds; i++) {
                curRowId = iRes1->rowIds[i];
                curRowId2 = iRes2->rowIds[i];
                if (Relation1->arrayOfInts[column1][curRowId] == Relation2->arrayOfInts[column2][curRowId2]) {
                    iRes1->rowIds[newResultsIndex] = curRowId;

                    // Change the joined relations aswell

                    for (int j = 0; j < iRes1->numJoinedRels; ++j) {
                        curiRes = iRes1->joinedRels[j];
                        curiRes->rowIds[newResultsIndex] = curiRes->rowIds[i];
                    }
                    iRes2->rowIds[newResultsIndex] = curRowId2;
                    for (int j = 0; j < iRes2->numJoinedRels; ++j) {
                        curiRes = iRes2->joinedRels[j];
                        curiRes->rowIds[newResultsIndex] = curiRes->rowIds[i];
                    }
                    newResultsIndex++;
                }
            }

            // Update relationIds intermediate result sizes already joined with relation 1
            for (int j = 0; j < iRes1->numJoinedRels; ++j) {
                curiRes = iRes1->joinedRels[j];
                intermediateResultUpdateSize(iResHead, curiRes->relationId, newResultsIndex);
            }
            // Update relationIds intermediate result sizes already joined with relation 2
            for (int j = 0; j < iRes2->numJoinedRels; ++j) {
                curiRes = iRes2->joinedRels[j];
                intermediateResultUpdateSize(iResHead, curiRes->relationId, newResultsIndex);
            }

            // Update relation 1 & 2 intermediate result sizes
            intermediateResultUpdateSize(iResHead, relationId1, newResultsIndex);
            intermediateResultUpdateSize(iResHead, relationId2, newResultsIndex);

            rDestroy(relation1);
            rDestroy(relation2);

            return;
        }

    /*
    // Different indexes but same relation (self join)
    if (Relation1->next == Relation2->next) {

        if (column1 != column2) {
            int isSelfJoinCase = 0;
            int numRowIds, numRowIds2;
            if (!isNewRes1 && isNewRes2) {
                numRowIds = iRes2->numRowIds;
                numRowIds2 = iRes1->numRowIds;
                isSelfJoinCase = 1;
            }
            else if (!isNewRes2 && isNewRes1) {
                numRowIds = iRes1->numRowIds;
                numRowIds2 = iRes2->numRowIds;
                isSelfJoinCase = 2;
            }
            if (isSelfJoinCase) {
                printf("Hi!\n");

                newResultsIndex = 0;
                uint64_t curRowId2;
                for (int i = 0; i < numRowIds; i++) {

                    if (isSelfJoinCase == 1) {
                        curRowId = iRes1->rowIds[i];
                        for (int j = 0; j < numRowIds2; j++) {
                            curRowId2 = j;
                            if (Relation1->arrayOfInts[column1][curRowId] == Relation2->arrayOfInts[column2][curRowId2]) {
                                iRes1->rowIds[newResultsIndex] = curRowId;

                                // Change the joined relations aswell

                                for (int j = 0; j < iRes1->numJoinedRels; ++j) {
                                    curiRes = iRes1->joinedRels[j];
                                    curiRes->rowIds[newResultsIndex] = curiRes->rowIds[i];
                                }
                                iRes2->rowIds[newResultsIndex] = curRowId2;
                                for (int j = 0; j < iRes2->numJoinedRels; ++j) {
                                    curiRes = iRes2->joinedRels[j];
                                    curiRes->rowIds[newResultsIndex] = curiRes->rowIds[i];
                                }
                                newResultsIndex++;
                            }
                        }

                    }
                    else {
                        curRowId = i;
                        for (int j = 0; j < numRowIds2; j++) {
                            curRowId2 = iRes2->rowIds[j];
                            if (Relation1->arrayOfInts[column1][curRowId] == Relation2->arrayOfInts[column2][curRowId2]) {
                                iRes1->rowIds[newResultsIndex] = curRowId;

                                // Change the joined relations aswell

                                if ()

                                for (int j = 0; j < iRes1->numJoinedRels; ++j) {
                                    curiRes = iRes1->joinedRels[j];
                                    curiRes->rowIds[newResultsIndex] = curiRes->rowIds[i];
                                }
                                iRes2->rowIds[newResultsIndex] = curRowId2;
                                for (int j = 0; j < iRes2->numJoinedRels; ++j) {
                                    curiRes = iRes2->joinedRels[j];
                                    curiRes->rowIds[newResultsIndex] = curiRes->rowIds[i];
                                }
                                newResultsIndex++;
                            }
                        }
                    }

                }
                // Update relationIds intermediate result sizes already joined with relation 1
                for (int j = 0; j < iRes1->numJoinedRels; ++j) {
                    curiRes = iRes1->joinedRels[j];
                    intermediateResultUpdateSize(iResHead, curiRes->relationId, newResultsIndex);
                }
                // Update relationIds intermediate result sizes already joined with relation 2
                for (int j = 0; j < iRes2->numJoinedRels; ++j) {
                    curiRes = iRes2->joinedRels[j];
                    intermediateResultUpdateSize(iResHead, curiRes->relationId, newResultsIndex);
                }

                // Update relation 1 & 2 intermediate result sizes
                intermediateResultUpdateSize(iResHead, relationId1, newResultsIndex);
                intermediateResultUpdateSize(iResHead, relationId2, newResultsIndex);

                rDestroy(relation1);
                rDestroy(relation2);

                return;
            }
        }
        */


        // If we reach this point, the 2 relations will be joined with radix hash join

        ResultHead *res;
        Result *cur;

        res = RadixHashJoin(relation1, relation2);
        // Result tuples depend on which relation has more tuples to check

        int totalTuples = resultGetTotalTuples(res);
        int numJoinedRel1 = iRes1->numJoinedRels;
        int numJoinedRel2 = iRes2->numJoinedRels;

        // Creating arrays to save intermediate results.
        // First array indexes point to rel1 / rel2 joined intermediate results, if there are any.
        // Last array index points to rel1 / rel2 intermediate results.

        uint64_t **rowIds1 = malloc((numJoinedRel1 + 1)* sizeof(uint64_t*));
        NULL_POINTER_ERROR_CHECK(rowIds1, __FILE__, __func__, __LINE__);
        for (int m = 0; m < numJoinedRel1 + 1; ++m) {
            rowIds1[m] = malloc(totalTuples * sizeof(uint64_t));
            NULL_POINTER_ERROR_CHECK(rowIds1[m], __FILE__, __func__, __LINE__);
        }

        uint64_t **rowIds2 = malloc((numJoinedRel2 + 1) * sizeof(uint64_t*));
        NULL_POINTER_ERROR_CHECK(rowIds2, __FILE__, __func__, __LINE__);
        for (int m = 0; m < numJoinedRel2 + 1; ++m) {
            rowIds2[m] = malloc(totalTuples * sizeof(uint64_t));
            NULL_POINTER_ERROR_CHECK(rowIds2[m], __FILE__, __func__, __LINE__);
        }

        uint64_t *tmpPtr;

        // In this case, first result rowIds inside tuple are from Relation 1
        if (numTuplesRel1 <= numTuplesRel2) {
            cur = res->first;
            newResultsIndex = 0;
            while (cur != NULL) {
                for (int i = 0; i < cur->numOfTuples; ++i) {
                    // If first occurance of Relation 1 then result rowIds are from the actual Relation1
                    if (isNewRes1) {
                        rowIds1[numJoinedRel1][newResultsIndex] = cur->buffer[i].rowId1;
                    }
                    // Else there are from the previous intermediate results
                    else {
                        rowIds1[numJoinedRel1][newResultsIndex] = iRes1->rowIds[cur->buffer[i].rowId1];
                    }
                    // Update the joined relations if there are any
                    for (int j = 0; j < numJoinedRel1; ++j) {
                        rowIds1[j][newResultsIndex] = iRes1->joinedRels[j]->rowIds[cur->buffer[i].rowId1];
                    }

                    // Same for second Relation

                    if (isNewRes2) {
                        rowIds2[numJoinedRel2][newResultsIndex] = cur->buffer[i].rowId2;
                    }
                    else {
                        rowIds2[numJoinedRel2][newResultsIndex] = iRes2->rowIds[cur->buffer[i].rowId2];
                    }
                    for (int j = 0; j < numJoinedRel2; ++j) {
                        rowIds2[j][newResultsIndex] = iRes2->joinedRels[j]->rowIds[cur->buffer[i].rowId2];
                    }

                    newResultsIndex++;
                }
                cur = cur->next;
            }
        }
        // In this case, first result rowIds inside tuple are from Relation 2
        else {
            cur = res->first;
            newResultsIndex = 0;
            while (cur != NULL) {
                for (int i = 0; i < cur->numOfTuples; ++i) {
                    // If first occurance of Relation 1 then result rowIds are from the actual Relation1
                    if (isNewRes1) {
                        rowIds1[numJoinedRel1][newResultsIndex] = cur->buffer[i].rowId2;
                    }
                    // Else there are from the previous intermediate results
                    else {
                        rowIds1[numJoinedRel1][newResultsIndex] = iRes1->rowIds[cur->buffer[i].rowId2];
                    }
                    // Update the joined relations if there are any
                    for (int j = 0; j < numJoinedRel1; ++j) {
                        rowIds1[j][newResultsIndex] = iRes1->joinedRels[j]->rowIds[cur->buffer[i].rowId2];
                    }

                    // Same for second Relation

                    if (isNewRes2) {
                        rowIds2[numJoinedRel2][newResultsIndex] = cur->buffer[i].rowId1;
                    }
                    else {
                        rowIds2[numJoinedRel2][newResultsIndex] = iRes2->rowIds[cur->buffer[i].rowId1];
                    }
                    for (int j = 0; j < numJoinedRel2; ++j) {
                        rowIds2[j][newResultsIndex] = iRes2->joinedRels[j]->rowIds[cur->buffer[i].rowId1];
                    }

                    newResultsIndex++;
                }
                cur = cur->next;
            }
        }

        // Update relations joined with relation 1
        for (int j = 0; j < iRes1->numJoinedRels; ++j) {
            curiRes = iRes1->joinedRels[j];

            // Free old intermediate results, update with the new ones.
            tmpPtr = curiRes->rowIds;
            curiRes->rowIds = rowIds1[j];
            free(tmpPtr);

            intermediateResultUpdateSize(iResHead, curiRes->relationId, newResultsIndex);
            intermediateResultAddJoinedRel(curiRes, iRes2);
            for (int i = 0; i < iRes2->numJoinedRels; ++i) {
                intermediateResultAddJoinedRel(curiRes, iRes2->joinedRels[i]);
            }
        }

        // Update relations joined with relation 2
        for (int j = 0; j < iRes2->numJoinedRels; ++j) {
            curiRes = iRes2->joinedRels[j];

            // Free old intermediate results, update with the new ones.
            tmpPtr = curiRes->rowIds;
            curiRes->rowIds = rowIds2[j];
            free(tmpPtr);

            intermediateResultUpdateSize(iResHead, curiRes->relationId, newResultsIndex);
            intermediateResultAddJoinedRel(curiRes, iRes1);
            for (int i = 0; i < iRes1->numJoinedRels; ++i) {
                intermediateResultAddJoinedRel(curiRes, iRes1->joinedRels[i]);
            }
        }

        // Copy joined rels of rel1 so that we can update relation 2 later
        int tmpNumRel1Joins = iRes1->numJoinedRels;
        IntermediateResult *rel1Joins[tmpNumRel1Joins];
        for (int k = 0; k < tmpNumRel1Joins ; ++k) {
            rel1Joins[k] = iRes1->joinedRels[k];
        }

        // Update relation 1

        // Free old intermediate results, update with the new ones.
        tmpPtr = iRes1->rowIds;
        iRes1->rowIds = rowIds1[numJoinedRel1];
        free(tmpPtr);

        intermediateResultUpdateSize(iResHead, relationId1, newResultsIndex);
        intermediateResultAddJoinedRel(iRes1, iRes2);
        for (int i = 0; i < iRes2->numJoinedRels; ++i) {
            curiRes = iRes2->joinedRels[i];
            intermediateResultAddJoinedRel(iRes1, curiRes);
        }

        // Update relation 2

        // Free old intermediate results, update with the new ones.
        tmpPtr = iRes2->rowIds;
        iRes2->rowIds = rowIds2[numJoinedRel2];
        free(tmpPtr);

        intermediateResultUpdateSize(iResHead, relationId2, newResultsIndex);
        intermediateResultAddJoinedRel(iRes2, iRes1);
        for (int i = 0; i < tmpNumRel1Joins; ++i) {
            curiRes = rel1Joins[i];
            intermediateResultAddJoinedRel(iRes2, curiRes);
        }

        free(rowIds1);
        free(rowIds2);

        rDestroy(relation1);
        rDestroy(relation2);
    }
}


void ProjectionJob(void *args) {

    // Input: int projIndex

    int i, projIndex, relId, colId;

    projIndex = ((int*) args)[0];

    // Saving global variables locally for less assembly code
    IntermediateResultHead *const intResHeadLocal = intResHead;
    ArrayList *const*const indexListNodesLocal = indexListNodes;
    uint64_t *const countersLocal = counters;


    uint64_t curRowId;
    IntermediateResult *iRes;

    relId = projectionArray[0][projIndex];
    colId = projectionArray[1][projIndex];

    iRes = intermediateResultGetNode(intResHeadLocal, relId);

    countersLocal[projIndex] = 0;

    // If relId has no intermediate node, add all rowIds of relation
    if (iRes == NULL) {
        for (i = 0; i < indexListNodesLocal[relId]->numOfRows; i++) {
            countersLocal[projIndex] += indexListNodesLocal[relId]->arrayOfInts[colId][i];
        }
    }
    else {
        for (i = 0; i < iRes->numRowIds; i++) {
            curRowId = iRes->rowIds[i];
            countersLocal[projIndex] += indexListNodesLocal[relId]->arrayOfInts[colId][curRowId];
        }
    }
}

void SQL_runProjections(char *projections, ArrayList **indexedListNodes, IntermediateResultHead *iResHead) {

    // Saving global variables locally for less assembly code
    JobScheduler *const jobSchedulerLocal = jobScheduler;

    intResHead = iResHead;
    indexListNodes = indexedListNodes;

    Job curJob;
    int i, numOfProjections, numJobs;
    char *token;

    numOfProjections = getSpaces(projections) + 1;

    counters = malloc(numOfProjections * sizeof(uint64_t));
    NULL_POINTER_ERROR_CHECK(counters, __FILE__, __func__, __LINE__);
    const uint64_t *const countersLocal = counters;

    projectionArray[0] = malloc(numOfProjections * sizeof(int));
    NULL_POINTER_ERROR_CHECK(projectionArray[0], __FILE__, __func__, __LINE__);
    projectionArray[1] = malloc(numOfProjections * sizeof(int));
    NULL_POINTER_ERROR_CHECK(projectionArray[1], __FILE__, __func__, __LINE__);

    numJobs = numOfProjections;
    jobSchedulerLocal->numJobs = numJobs;

    // Calculate sum of projections using threads

    i = 0;
    token = strtok(projections,".");
    curJob.jobRoutine = ProjectionJob;
    while (token != NULL) {
        projectionArray[0][i] = atoi(token);
        token = strtok(NULL, " ");
        projectionArray[1][i] = atoi(token);
        token = strtok(NULL, ".");

        curJob.arguments = malloc(sizeof(int));
        NULL_POINTER_ERROR_CHECK(curJob.arguments, __FILE__, __func__, __LINE__);

        memcpy(curJob.arguments, &i, sizeof(int));

        jsSchedule(curJob);
        i++;
    }

    jsBarrier();

    // Print results
#ifdef HARNESS
    char *tempBuff = (char *)malloc(1000*sizeof(char));
    NULL_POINTER_ERROR_CHECK(tempBuff, __FILE__, __func__, __LINE__);
    memset(tempBuff, '\0', 1000*sizeof(char));

    for (i = 0; i < numJobs; i++) {

        if (countersLocal[i] == 0) {
            sprintf(tempBuff + strlen(tempBuff), "NULL");
        }
        else {
            sprintf(tempBuff + strlen(tempBuff), "%lu", countersLocal[i]);
        }

        if (i < numJobs - 1) {
            sprintf(tempBuff + strlen(tempBuff), " ");
        }
    }

    sprintf(tempBuff + strlen(tempBuff), "\n");
    ssize_t status_bytes = write(1, tempBuff, strlen(tempBuff));
    NEGATIVE_VALUE_ERROR_CHECK(status_bytes, __FILE__, __func__, __LINE__);

    free(tempBuff);
#else
    for (i = 0; i < numJobs; i++) {

        if (countersLocal[i] == 0) {
            printf("NULL");
        }
        else {
            printf("%lu", countersLocal[i]);
        }

        if (i < numJobs - 1) {
            printf(" ");
        }
    }

    printf("\n");
#endif


    free(projectionArray[0]);
    free(projectionArray[1]);
    free(counters);
}