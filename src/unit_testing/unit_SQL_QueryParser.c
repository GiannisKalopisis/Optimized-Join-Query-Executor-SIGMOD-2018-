//
// Created by kalo-pc on 23/11/2018.
//

#include <stdio.h>
#include <stdlib.h>

#include "../../include/unit_SQL_QueryParser.h"
#include "../../include/SQL_QueryParser.h"
#include "../../include/relations.h"
#include "../../include/result.h"
#include "../../include/error_check.h"
#include "CUnit/Basic.h"


#include "../include/general_functions.h"
#include "../include/RHJ.h"
#include "../include/predicateParser.h"


void testMainQueryParser(void) {

    /* variable declaration */
    char *query = NULL, *relations = NULL, *predicates = NULL;
    char *token = NULL, *projections = NULL, **projArray = NULL;
    int numOfRelations = 0;
    int i = 0, numOfProjections= 0;
    int relId, colId;
    IntermediateResult *iRes = NULL;
    ArrayHead *headRelationList = NULL;
    uint64_t counter = 0, curRowId = 0;
    ArrayList **indexedListNodes = NULL;
    IntermediateResultHead *iResHead = NULL;
    predicateListHead *predicateHead = NULL;
    predicateList *curPredicate = NULL;


    /*   Our binary test files
     *____________________________
     *      testBinaryData1
     *
     *   3 columns with 4 rows
     *
     *      1,  2,  3,  4
     *      5,  6,  7,  8
     *      9, 10, 11, 12
     * ___________________________
     *      testBinaryData2
     *
     *    2 columns with 6 rows
     *
     *  10, 20, 30,  40,  50,  60
     *  70, 80, 90, 100, 110, 120
     *____________________________
     *      testBinaryData3
     *
     *   4 columns with 5 rows
     *
     *     1, 10,  2, 20,   3
     *    30,  4, 40,  5,  50
     *     6, 60,  7, 70,   8
     *    80,  9, 90, 10, 100
     *___________________________
     *
     */


    /*test 1*/
    query = (char *) malloc((strlen("0 1|0.1>20&1.0=10|0.1 0.2") + 1) * sizeof(char));
    strcpy(query, "0 1|0.1>20&1.0=10|0.1 0.2");
    relations = strtok(query, "|");
    predicates = strtok(NULL, "|");
    projections = strtok(NULL, "|\0\n");
    numOfRelations = getSpaces(relations);
    numOfRelations++;
    arrayHeadInit(&headRelationList);
    arrayListInsert(headRelationList, "./test_txts/testBinaryData1.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData2.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData3.bin");
    indexedListNodes = SQL_arrayIndexing(relations, headRelationList, numOfRelations);
    intermediateResultInit(&iResHead);
    predicateListHeadInit(&predicateHead);
    predicateListCreation(predicateHead, predicates);
    predicateHead->first->priority = 2;
    predicateHead->last->priority = 3;
    while ( (curPredicate = getHighestPriorityPredicate(predicateHead)) != NULL ) {
        if (curPredicate->isFilter) {
            switch (curPredicate->isFilter) {
                case 1: // >
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '>', curPredicate->filterValue, iResHead);
                    break;
                case 2: // <
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '<', curPredicate->filterValue, iResHead);
                    break;
                case 3: // =
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '=', curPredicate->filterValue, iResHead);
                    break;
            }
            //updatePriority(predicateHead, curPredicate->array1, -1);
        }
        // Conjunction query
        else {
            SQL_runConjuctionPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1,
                                       indexedListNodes[curPredicate->array2], curPredicate->array2, curPredicate->columnOfArray2, iResHead);
            //updatePriority(predicateHead, curPredicate->array1, curPredicate->array2);
        }
    }

    numOfProjections = getSpaces(projections) + 1;
    projArray = (char **) malloc(numOfProjections * sizeof(char*));
    NULL_POINTER_ERROR_CHECK(projArray, __FILE__, __func__, __LINE__);
    i = 0;
    token = strtok(projections," ");
    while (token != NULL) {
        projArray[i] = token;
        token = strtok(NULL, " ");
        i++;
    }

    token = strtok(projArray[0], ".");
    relId = atoi(token);
    token = strtok(NULL, ".");
    colId = atoi(token);
    iRes = intermediateResultGetNode(iResHead, relId);
    counter = 0;
    for (int k = 0; k < iRes->numRowIds; ++k) {
        curRowId = iRes->rowIds[k];
        counter += indexedListNodes[relId]->arrayOfInts[colId][curRowId];
    }
    CU_ASSERT(counter == 0);
    token = strtok(projArray[1], ".");
    relId = atoi(token);
    token = strtok(NULL, ".");
    colId = atoi(token);
    iRes = intermediateResultGetNode(iResHead, relId);
    counter = 0;
    for (int k = 0; k < iRes->numRowIds; ++k) {
        curRowId = iRes->rowIds[k];
        counter += indexedListNodes[relId]->arrayOfInts[colId][curRowId];
    }
    CU_ASSERT(counter == 0);

    free(projArray);

    free(indexedListNodes);
    intermediateResultDestroy(iResHead);
    free(query);
    DestroyPredicateList(predicateHead);
    arrayListDestroy(headRelationList);




    /*test 2*/
    query = (char *) malloc((strlen("2|0.3<50|0.0 0.1") + 1) * sizeof(char));
    strcpy(query, "2|0.3<50|0.0 0.1");
    relations = strtok(query, "|");
    predicates = strtok(NULL, "|");
    projections = strtok(NULL, "|\0\n");
    numOfRelations = getSpaces(relations);
    numOfRelations++;
    arrayHeadInit(&headRelationList);
    arrayListInsert(headRelationList, "./test_txts/testBinaryData1.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData2.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData3.bin");
    indexedListNodes = SQL_arrayIndexing(relations, headRelationList, numOfRelations);
    intermediateResultInit(&iResHead);
    predicateListHeadInit(&predicateHead);
    predicateListCreation(predicateHead, predicates);
    predicateHead->first->priority = 1;
    while ( (curPredicate = getHighestPriorityPredicate(predicateHead)) != NULL ) {
        if (curPredicate->isFilter) {
            switch (curPredicate->isFilter) {
                case 1: // >
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '>', curPredicate->filterValue, iResHead);
                    break;
                case 2: // <
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '<', curPredicate->filterValue, iResHead);
                    break;
                case 3: // =
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '=', curPredicate->filterValue, iResHead);
                    break;
            }
            //updatePriority(predicateHead, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            SQL_runConjuctionPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1,
                                       indexedListNodes[curPredicate->array2], curPredicate->array2, curPredicate->columnOfArray2, iResHead);
            //updatePriority(predicateHead, curPredicate->array1, curPredicate->array2);
        }
    }

    numOfProjections = getSpaces(projections) + 1;
    projArray = (char **) malloc(numOfProjections * sizeof(char*));
    NULL_POINTER_ERROR_CHECK(projArray, __FILE__, __func__, __LINE__);
    i = 0;
    token = strtok(projections," ");
    while (token != NULL) {
        projArray[i] = token;
        token = strtok(NULL, " ");
        i++;
    }

    token = strtok(projArray[0], ".");
    relId = atoi(token);
    token = strtok(NULL, ".");
    colId = atoi(token);
    iRes = intermediateResultGetNode(iResHead, relId);
    counter = 0;
    for (int k = 0; k < iRes->numRowIds; ++k) {
        curRowId = iRes->rowIds[k];
        counter += indexedListNodes[relId]->arrayOfInts[colId][curRowId];
    }
    CU_ASSERT(counter == 30);
    token = strtok(projArray[1], ".");
    relId = atoi(token);
    token = strtok(NULL, ".");
    colId = atoi(token);
    iRes = intermediateResultGetNode(iResHead, relId);
    counter = 0;
    for (int k = 0; k < iRes->numRowIds; ++k) {
        curRowId = iRes->rowIds[k];
        counter += indexedListNodes[relId]->arrayOfInts[colId][curRowId];
    }
    CU_ASSERT(counter == 9);

    free(projArray);

    free(indexedListNodes);
    intermediateResultDestroy(iResHead);
    free(query);
    DestroyPredicateList(predicateHead);
    arrayListDestroy(headRelationList);



    /*test 3*/
    query = (char *) malloc((strlen("1 0 2|2.1>5&0.1=1.2&2.3<50|2.0 0.2 1.1") + 1) * sizeof(char));
    strcpy(query, "1 0 2|2.1>5&0.1=1.2&2.3<50|2.0 0.2 1.1");
    relations = strtok(query, "|");
    predicates = strtok(NULL, "|");
    projections = strtok(NULL, "|\0\n");
    numOfRelations = getSpaces(relations);
    numOfRelations++;
    arrayHeadInit(&headRelationList);
    arrayListInsert(headRelationList, "./test_txts/testBinaryData1.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData2.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData3.bin");
    indexedListNodes = SQL_arrayIndexing(relations, headRelationList, numOfRelations);
    intermediateResultInit(&iResHead);
    predicateListHeadInit(&predicateHead);
    predicateListCreation(predicateHead, predicates);
    predicateHead->first->priority = 3;
    predicateHead->first->next->priority = 2;
    predicateHead->last->priority = 3;
    while ( (curPredicate = getHighestPriorityPredicate(predicateHead)) != NULL ) {
        if (curPredicate->isFilter) {
            switch (curPredicate->isFilter) {
                case 1: // >
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '>', curPredicate->filterValue, iResHead);
                    break;
                case 2: // <
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '<', curPredicate->filterValue, iResHead);
                    break;
                case 3: // =
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '=', curPredicate->filterValue, iResHead);
                    break;
            }
            //updatePriority(predicateHead, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            SQL_runConjuctionPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1,
                                       indexedListNodes[curPredicate->array2], curPredicate->array2, curPredicate->columnOfArray2, iResHead);
            //updatePriority(predicateHead, curPredicate->array1, curPredicate->array2);
        }
    }

    numOfProjections = getSpaces(projections) + 1;
    projArray = (char **) malloc(numOfProjections * sizeof(char*));
    NULL_POINTER_ERROR_CHECK(projArray, __FILE__, __func__, __LINE__);
    i = 0;
    token = strtok(projections," ");
    while (token != NULL) {
        projArray[i] = token;
        token = strtok(NULL, " ");
        i++;
    }

    token = strtok(projArray[0], ".");
    relId = atoi(token);
    token = strtok(NULL, ".");
    colId = atoi(token);
    iRes = intermediateResultGetNode(iResHead, relId);
    counter = 0;
    for (int k = 0; k < iRes->numRowIds; ++k) {
        curRowId = iRes->rowIds[k];
        counter += indexedListNodes[relId]->arrayOfInts[colId][curRowId];
    }
    CU_ASSERT(counter == 0);
    token = strtok(projArray[1], ".");
    relId = atoi(token);
    token = strtok(NULL, ".");
    colId = atoi(token);
    iRes = intermediateResultGetNode(iResHead, relId);
    counter = 0;
    for (int k = 0; k < iRes->numRowIds; ++k) {
        curRowId = iRes->rowIds[k];
        counter += indexedListNodes[relId]->arrayOfInts[colId][curRowId];
    }
    CU_ASSERT(counter == 0);
    token = strtok(projArray[2], ".");
    relId = atoi(token);
    token = strtok(NULL, ".");
    colId = atoi(token);
    iRes = intermediateResultGetNode(iResHead, relId);
    counter = 0;
    for (int k = 0; k < iRes->numRowIds; ++k) {
        curRowId = iRes->rowIds[k];
        counter += indexedListNodes[relId]->arrayOfInts[colId][curRowId];
    }
    CU_ASSERT(counter == 0);

    free(projArray);

    free(indexedListNodes);
    intermediateResultDestroy(iResHead);
    free(query);
    DestroyPredicateList(predicateHead);
    arrayListDestroy(headRelationList);


    /*test 4*/
    query = (char *) malloc((strlen("2 0|0.0=1.0|0.0") + 1) * sizeof(char));
    strcpy(query, "2 0|0.0=1.0|0.0");
    relations = strtok(query, "|");
    predicates = strtok(NULL, "|");
    projections = strtok(NULL, "|\0\n");
    numOfRelations = getSpaces(relations);
    numOfRelations++;
    arrayHeadInit(&headRelationList);
    arrayListInsert(headRelationList, "./test_txts/testBinaryData1.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData2.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData3.bin");
    indexedListNodes = SQL_arrayIndexing(relations, headRelationList, numOfRelations);
    intermediateResultInit(&iResHead);
    predicateListHeadInit(&predicateHead);
    predicateListCreation(predicateHead, predicates);
    predicateHead->first->priority = 8;
    while ( (curPredicate = getHighestPriorityPredicate(predicateHead)) != NULL ) {
        if (curPredicate->isFilter) {
            switch (curPredicate->isFilter) {
                case 1: // >
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '>', curPredicate->filterValue, iResHead);
                    break;
                case 2: // <
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '<', curPredicate->filterValue, iResHead);
                    break;
                case 3: // =
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '=', curPredicate->filterValue, iResHead);
                    break;
            }
            //updatePriority(predicateHead, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            SQL_runConjuctionPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1,
                                       indexedListNodes[curPredicate->array2], curPredicate->array2, curPredicate->columnOfArray2, iResHead);
            //updatePriority(predicateHead, curPredicate->array1, curPredicate->array2);
        }
    }

    numOfProjections = getSpaces(projections) + 1;
    projArray = (char **) malloc(numOfProjections * sizeof(char*));
    NULL_POINTER_ERROR_CHECK(projArray, __FILE__, __func__, __LINE__);
    i = 0;
    token = strtok(projections," ");
    while (token != NULL) {
        projArray[i] = token;
        token = strtok(NULL, " ");
        i++;
    }

    token = strtok(projArray[0], ".");
    relId = atoi(token);
    token = strtok(NULL, ".");
    colId = atoi(token);
    iRes = intermediateResultGetNode(iResHead, relId);
    counter = 0;
    for (int k = 0; k < iRes->numRowIds; ++k) {
        curRowId = iRes->rowIds[k];
        counter += indexedListNodes[relId]->arrayOfInts[colId][curRowId];
    }
    CU_ASSERT(counter == 6);

    free(projArray);

    free(indexedListNodes);
    intermediateResultDestroy(iResHead);
    free(query);
    DestroyPredicateList(predicateHead);
    arrayListDestroy(headRelationList);



    /*test 5*/
    query = (char *) malloc((strlen("1 2|0.1=1.3|1.1") + 1) * sizeof(char));
    strcpy(query, "1 2|0.1=1.3|1.1");
    relations = strtok(query, "|");
    predicates = strtok(NULL, "|");
    projections = strtok(NULL, "|\0\n");
    numOfRelations = getSpaces(relations);
    numOfRelations++;
    arrayHeadInit(&headRelationList);
    arrayListInsert(headRelationList, "./test_txts/testBinaryData1.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData2.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData3.bin");
    indexedListNodes = SQL_arrayIndexing(relations, headRelationList, numOfRelations);
    intermediateResultInit(&iResHead);
    predicateListHeadInit(&predicateHead);
    predicateListCreation(predicateHead, predicates);
    predicateHead->first->priority = 1;
    while ( (curPredicate = getHighestPriorityPredicate(predicateHead)) != NULL ) {
        if (curPredicate->isFilter) {
            switch (curPredicate->isFilter) {
                case 1: // >
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '>', curPredicate->filterValue, iResHead);
                    break;
                case 2: // <
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '<', curPredicate->filterValue, iResHead);
                    break;
                case 3: // =
                    SQL_runFilterPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1, '=', curPredicate->filterValue, iResHead);
                    break;
            }
            //updatePriority(predicateHead, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            SQL_runConjuctionPredicate(indexedListNodes[curPredicate->array1], curPredicate->array1, curPredicate->columnOfArray1,
                                       indexedListNodes[curPredicate->array2], curPredicate->array2, curPredicate->columnOfArray2, iResHead);
            //updatePriority(predicateHead, curPredicate->array1, curPredicate->array2);
        }
    }

    numOfProjections = getSpaces(projections) + 1;
    projArray = (char **) malloc(numOfProjections * sizeof(char*));
    NULL_POINTER_ERROR_CHECK(projArray, __FILE__, __func__, __LINE__);
    i = 0;
    token = strtok(projections," ");
    while (token != NULL) {
        projArray[i] = token;
        token = strtok(NULL, " ");
        i++;
    }

    token = strtok(projArray[0], ".");
    relId = atoi(token);
    token = strtok(NULL, ".");
    colId = atoi(token);
    iRes = intermediateResultGetNode(iResHead, relId);
    counter = 0;
    for (int k = 0; k < iRes->numRowIds; ++k) {
        curRowId = iRes->rowIds[k];
        counter += indexedListNodes[relId]->arrayOfInts[colId][curRowId];
    }
    CU_ASSERT(counter == 120);
    free(projArray);

    free(indexedListNodes);
    intermediateResultDestroy(iResHead);
    free(query);
    DestroyPredicateList(predicateHead);
    arrayListDestroy(headRelationList);

}

void testSQL_arrayIndexing(void) {

    /*

    FILE *fp = fopen("testBinaryData3.bin", "wb");
    uint64_t x1[5] = {1,10,2,20,3};
    uint64_t x2[5] = {30,4,40,5,50};
    uint64_t x3[5] = {6,60,7,70,8};
    uint64_t x4[5] = {80,9,90,10,100};

    uint64_t col = 4;
    uint64_t row = 5;

    fwrite(&row, sizeof(uint64_t), 1, fp);
    fwrite(&col, sizeof(uint64_t), 1, fp);

    fwrite(x1, sizeof(uint64_t), 5, fp);
    fwrite(x2, sizeof(uint64_t), 5, fp);
    fwrite(x3, sizeof(uint64_t), 5, fp);
    fwrite(x4, sizeof(uint64_t), 5, fp);

    fclose(fp);

    */

    ArrayHead *headRelationList;
    arrayHeadInit(&headRelationList);

    // ArrayList functions will be tested before this so there is no problem using them here
    arrayListInsert(headRelationList, "./test_txts/testBinaryData1.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData2.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData3.bin");

    ArrayList **indexedListNodes = NULL;

    //// Test 1 ////

    indexedListNodes = SQL_arrayIndexing("0 2", headRelationList, 2);

    CU_ASSERT(indexedListNodes[0] == headRelationList->first);
    CU_ASSERT(indexedListNodes[1] == headRelationList->last);

    free(indexedListNodes);
    indexedListNodes = NULL;


    //// Test 2 ////

    indexedListNodes = SQL_arrayIndexing("2 0", headRelationList, 2);

    CU_ASSERT(indexedListNodes[0] == headRelationList->last);
    CU_ASSERT(indexedListNodes[1] == headRelationList->first);

    free(indexedListNodes);
    indexedListNodes = NULL;


    //// Test 3 ////

    indexedListNodes = SQL_arrayIndexing("1 2 0", headRelationList, 3);

    CU_ASSERT(indexedListNodes[0] == headRelationList->first->next);
    CU_ASSERT(indexedListNodes[1] == headRelationList->last);
    CU_ASSERT(indexedListNodes[2] == headRelationList->first);

    free(indexedListNodes);
    indexedListNodes = NULL;

    //// Test 4 ////

    indexedListNodes = SQL_arrayIndexing("2 0 1", headRelationList, 3);

    CU_ASSERT(indexedListNodes[0] == headRelationList->last);
    CU_ASSERT(indexedListNodes[1] == headRelationList->first);
    CU_ASSERT(indexedListNodes[2] == headRelationList->first->next);

    free(indexedListNodes);
    indexedListNodes = NULL;

    //// Test 5 ////

    indexedListNodes = SQL_arrayIndexing("1", headRelationList, 1);

    CU_ASSERT(indexedListNodes[0] == headRelationList->first->next);


    // Maybe add test for empty first argument?

    free(indexedListNodes);

    arrayListDestroy(headRelationList);

}


void testSQL_runFilterPredicate(void) {

    /*   Our binary test files
     *____________________________
     *      testBinaryData1
     *
     *   3 columns with 4 rows
     *
     *      1,  2,  3,  4
     *      5,  6,  7,  8
     *      9, 10, 11, 12
     *
     * ___________________________
     *      testBinaryData2
     *
     *    2 columns with 6 rows
     *
     *  10, 20, 30,  40,  50,  60
     *  70, 80, 90, 100, 110, 120
     *
     *____________________________
     *      testBinaryData3
     *
     *   4 columns with 5 rows
     *
     *     1, 10,  2, 20,   3
     *    30,  4, 40,  5,  50
     *     6, 60,  7, 70,   8
     *    80,  9, 90, 10, 100
     *___________________________
     *
     */


    ArrayHead *headRelationList;
    arrayHeadInit(&headRelationList);

    // ArrayList functions will be tested before this so there is no problem using them here
    arrayListInsert(headRelationList, "./test_txts/testBinaryData1.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData2.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData3.bin");

    ArrayList **indexedListNodes = NULL;

    indexedListNodes = SQL_arrayIndexing("0 1 2", headRelationList, 3);

    IntermediateResultHead *iResHead;
    intermediateResultInit(&iResHead);
    // IntermediateResult functions will be tested before this so there is no problem using them here

    //// Test 1 - '>' operator, checking it individually ////

    // 0.1>4
    SQL_runFilterPredicate(indexedListNodes[0], 0, 1, '>', 4, iResHead);


    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 4);
    CU_ASSERT(iResHead->first->rowIds[0] == 0);
    CU_ASSERT(iResHead->first->rowIds[1] == 1);
    CU_ASSERT(iResHead->first->rowIds[2] == 2);
    CU_ASSERT(iResHead->first->rowIds[3] == 3);

    // 0.1>6
    SQL_runFilterPredicate(indexedListNodes[0], 0, 1, '>', 6, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 2);
    CU_ASSERT(iResHead->first->rowIds[0] == 2);
    CU_ASSERT(iResHead->first->rowIds[1] == 3);

    // 0.1>8
    SQL_runFilterPredicate(indexedListNodes[0], 0, 1, '>', 8, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 0);


    intermediateResultDestroy(iResHead);
    intermediateResultInit(&iResHead);

    //// Test 2 - '<' operator, checking it individually ////

    // 0.1<8
    SQL_runFilterPredicate(indexedListNodes[0], 0, 1, '<', 8, iResHead);


    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 3);
    CU_ASSERT(iResHead->first->rowIds[0] == 0);
    CU_ASSERT(iResHead->first->rowIds[1] == 1);
    CU_ASSERT(iResHead->first->rowIds[2] == 2);

    // 0.1<6
    SQL_runFilterPredicate(indexedListNodes[0], 0, 1, '<', 6, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 1);
    CU_ASSERT(iResHead->first->rowIds[0] == 0);

    // 0.1<5
    SQL_runFilterPredicate(indexedListNodes[0], 0, 1, '<', 5, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 0);

    intermediateResultDestroy(iResHead);
    intermediateResultInit(&iResHead);

    //// Test 3 - '=' operator, checking it individually ////

    // 0.1=6
    SQL_runFilterPredicate(indexedListNodes[0], 0, 1, '=', 6, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 1);
    CU_ASSERT(iResHead->first->rowIds[0] == 1);

    // 0.1=7
    SQL_runFilterPredicate(indexedListNodes[0], 0, 1, '=', 7, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 0);

    intermediateResultDestroy(iResHead);
    intermediateResultInit(&iResHead);

    //// Test 4, multiple operators, same column ////

    // 0.2<12
    SQL_runFilterPredicate(indexedListNodes[0], 0, 2, '<', 12, iResHead);


    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 3);
    CU_ASSERT(iResHead->first->rowIds[0] == 0);
    CU_ASSERT(iResHead->first->rowIds[1] == 1);
    CU_ASSERT(iResHead->first->rowIds[2] == 2);

    // 0.2>9
    SQL_runFilterPredicate(indexedListNodes[0], 0, 2, '>', 9, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 2);
    CU_ASSERT(iResHead->first->rowIds[0] == 1);
    CU_ASSERT(iResHead->first->rowIds[1] == 2);

    // 0.2=11
    SQL_runFilterPredicate(indexedListNodes[0], 0, 1, '=', 11, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 0);

    intermediateResultDestroy(iResHead);
    intermediateResultInit(&iResHead);

    //// Test 5, final test - little bit of everything ////

    // 1.1<100
    SQL_runFilterPredicate(indexedListNodes[1], 1, 1, '<', 100, iResHead);


    CU_ASSERT(iResHead->first->relationId == 1);
    CU_ASSERT(iResHead->first->numRowIds == 3);
    CU_ASSERT(iResHead->first->rowIds[0] == 0);
    CU_ASSERT(iResHead->first->rowIds[1] == 1);
    CU_ASSERT(iResHead->first->rowIds[2] == 2);

    // 1.0>10
    SQL_runFilterPredicate(indexedListNodes[1], 1, 0, '>', 10, iResHead);

    CU_ASSERT(iResHead->first->relationId == 1);
    CU_ASSERT(iResHead->first->numRowIds == 2);
    CU_ASSERT(iResHead->first->rowIds[0] == 1);
    CU_ASSERT(iResHead->first->rowIds[1] == 2);

    // 2.0<20
    SQL_runFilterPredicate(indexedListNodes[2], 2, 0, '<', 20, iResHead);

    CU_ASSERT(iResHead->first->next->relationId == 2);
    CU_ASSERT(iResHead->first->next->numRowIds == 4);
    CU_ASSERT(iResHead->first->next->rowIds[0] == 0);
    CU_ASSERT(iResHead->first->next->rowIds[1] == 1);
    CU_ASSERT(iResHead->first->next->rowIds[2] == 2);
    CU_ASSERT(iResHead->first->next->rowIds[3] == 4);

    // 2.2=90
    SQL_runFilterPredicate(indexedListNodes[2], 2, 2, '=', 90, iResHead);

    CU_ASSERT(iResHead->first->next->relationId == 2);
    CU_ASSERT(iResHead->first->next->numRowIds == 0);

    intermediateResultDestroy(iResHead);

    free(indexedListNodes);

    arrayListDestroy(headRelationList);
}


void testSQL_runConjuctionPredicate(void) {

    /*   Our binary test files
     *____________________________
     *      testBinaryData1
     *
     *   3 columns with 4 rows
     *
     *      1,  2,  3,  4
     *      5,  6,  7,  8
     *      9, 10, 11, 12
     *
     * ___________________________
     *      testBinaryData2
     *
     *    2 columns with 6 rows
     *
     *  10, 20, 30,  40,  50,  60
     *  70, 80, 90, 100, 110, 120
     *
     *____________________________
     *      testBinaryData3
     *
     *   4 columns with 5 rows
     *
     *     1, 10,  2, 20,   3
     *    30,  4, 40,  5,  50
     *     6, 60,  7, 70,   8
     *    80,  9, 90, 10, 100
     *___________________________
     *
     */


    ArrayHead *headRelationList;
    arrayHeadInit(&headRelationList);

    // ArrayList functions will be tested before this so there is no problem using them here
    arrayListInsert(headRelationList, "./test_txts/testBinaryData1.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData2.bin");
    arrayListInsert(headRelationList, "./test_txts/testBinaryData3.bin");

    ArrayList **indexedListNodes = NULL;

    indexedListNodes = SQL_arrayIndexing("0 1 2", headRelationList, 3);

    IntermediateResultHead *iResHead;
    intermediateResultInit(&iResHead);
    // IntermediateResult functions will be tested before this so there is no problem using them here

    //// Test 1 - Same relation ////

    // 0.0=0.0
    SQL_runConjuctionPredicate(indexedListNodes[0], 0, 0, indexedListNodes[0], 0, 0, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 4);
    CU_ASSERT(iResHead->first->rowIds[0] == 0);
    CU_ASSERT(iResHead->first->rowIds[1] == 1);
    CU_ASSERT(iResHead->first->rowIds[2] == 2);
    CU_ASSERT(iResHead->first->rowIds[3] == 3);
    CU_ASSERT(iResHead->first->numJoinedRels == 0);
    CU_ASSERT(iResHead->first->joinedRels == NULL);

    // 0.0=0.1
    SQL_runConjuctionPredicate(indexedListNodes[0], 0, 0, indexedListNodes[0], 0, 1, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 0);
    CU_ASSERT(iResHead->first->numJoinedRels == 0);
    CU_ASSERT(iResHead->first->joinedRels == NULL);

    intermediateResultDestroy(iResHead);
    intermediateResultInit(&iResHead);

    //// Test 2 - reverse order of Test 1 ////

    // 0.0=0.1
    SQL_runConjuctionPredicate(indexedListNodes[0], 0, 0, indexedListNodes[0], 0, 1, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 0);
    CU_ASSERT(iResHead->first->numJoinedRels == 0);
    CU_ASSERT(iResHead->first->joinedRels == NULL);

    // 0.0=0.0
    SQL_runConjuctionPredicate(indexedListNodes[0], 0, 0, indexedListNodes[0], 0, 0, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 0);
    CU_ASSERT(iResHead->first->numJoinedRels == 0);
    CU_ASSERT(iResHead->first->joinedRels == NULL);

    intermediateResultDestroy(iResHead);
    intermediateResultInit(&iResHead);

    //// Test 3 - Different relations ////

    // 0.0=2.0
    SQL_runConjuctionPredicate(indexedListNodes[0], 0, 0, indexedListNodes[2], 2, 0, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 3);
    CU_ASSERT(iResHead->first->rowIds[0] == 0);
    CU_ASSERT(iResHead->first->rowIds[1] == 1);
    CU_ASSERT(iResHead->first->rowIds[2] == 2);
    CU_ASSERT(iResHead->first->numJoinedRels == 1);
    CU_ASSERT(iResHead->first->joinedRels[0]->relationId == 2);


    CU_ASSERT(iResHead->first->next->relationId == 2);
    CU_ASSERT(iResHead->first->next->numRowIds == 3);
    CU_ASSERT(iResHead->first->next->rowIds[0] == 0);
    CU_ASSERT(iResHead->first->next->rowIds[1] == 2);
    CU_ASSERT(iResHead->first->next->rowIds[2] == 4);
    CU_ASSERT(iResHead->first->next->numJoinedRels == 1);
    CU_ASSERT(iResHead->first->next->joinedRels[0]->relationId == 0);

    // 0.2=1.0
    SQL_runConjuctionPredicate(indexedListNodes[0], 0, 2, indexedListNodes[1], 1, 0, iResHead);

    CU_ASSERT(iResHead->first->relationId == 0);
    CU_ASSERT(iResHead->first->numRowIds == 1);
    CU_ASSERT(iResHead->first->rowIds[0] == 1);
    CU_ASSERT(iResHead->first->numJoinedRels == 2);
    CU_ASSERT(iResHead->first->joinedRels[0]->relationId == 2);
    CU_ASSERT(iResHead->first->joinedRels[1]->relationId == 1);


    CU_ASSERT(iResHead->first->next->relationId == 2);
    CU_ASSERT(iResHead->first->next->numRowIds == 1);
    CU_ASSERT(iResHead->first->next->rowIds[0] == 2);
    CU_ASSERT(iResHead->first->numJoinedRels == 2);
    CU_ASSERT(iResHead->first->next->joinedRels[0]->relationId == 0);
    CU_ASSERT(iResHead->first->next->joinedRels[1]->relationId == 1);

    CU_ASSERT(iResHead->last->relationId == 1);
    CU_ASSERT(iResHead->last->numRowIds == 1);
    CU_ASSERT(iResHead->last->rowIds[0] == 0);
    CU_ASSERT(iResHead->last->numJoinedRels == 2);
    CU_ASSERT(iResHead->last->joinedRels[0]->relationId == 0);
    CU_ASSERT(iResHead->last->joinedRels[1]->relationId == 2);


    intermediateResultDestroy(iResHead);

    free(indexedListNodes);

    arrayListDestroy(headRelationList);
}
