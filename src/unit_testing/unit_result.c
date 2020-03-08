#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../include/unit_result.h"
#include "../../include/result.h"
#include "CUnit/Basic.h"


void testResultInit(void) {

    ResultHead *res;
    resultInit(&res);
    CU_ASSERT_PTR_NOT_NULL(res);
    CU_ASSERT_PTR_NOT_NULL(res->last);
    CU_ASSERT_PTR_NOT_NULL(res->first);
    CU_ASSERT(res->maxTuples == (BUFFERSIZE / sizeof(ResultTuple)));
    resultDestroy(res);

}


void testResultCreateNewNode(void) {

    Result *resNode = resultCreateNewNode();
    CU_ASSERT_PTR_NOT_NULL(resNode);
    free(resNode->buffer);
    free(resNode);

}


void testResultIsLastNodeFull(void) {

    uint64_t resultRow1, resultRow2;

    //test 1
    ResultHead *res;
    resultInit(&res);
    CU_ASSERT_PTR_NOT_NULL(res);
    CU_ASSERT_PTR_NOT_NULL(res->last);
    CU_ASSERT_PTR_NOT_NULL(res->first);
    CU_ASSERT(res->maxTuples == (BUFFERSIZE / sizeof(ResultTuple)));

    for (int i = 0; i < res->maxTuples; ++i) {

        resultRow1 = i;
        resultRow2 = i;
        resultAppend(res, resultRow1, resultRow2);

        if (i == 0 || i == (res->maxTuples / 2) || i == (res->maxTuples - 100)) {
            CU_ASSERT(resultIsLastNodeFull(res) == 0);
        }

        if (i == (res->maxTuples - 1)){
            CU_ASSERT(res->maxTuples == res->last->numOfTuples);
            CU_ASSERT(resultIsLastNodeFull(res) == 1);
        }
    }

    resultDestroy(res);

}


void testResultAppend(void) {

    uint64_t resultRow1, resultRow2;

    //test 1
    ResultHead *res;
    resultInit(&res);
    CU_ASSERT_PTR_NOT_NULL(res);
    CU_ASSERT_PTR_NOT_NULL(res->last);
    CU_ASSERT_PTR_NOT_NULL(res->first);
    CU_ASSERT(res->maxTuples == (BUFFERSIZE / sizeof(ResultTuple)));

    Result *lastNode = NULL;
    int index = 0;

    for (int i = 0; i < ((res->maxTuples) * 2); ++i) {

        lastNode = res->last;
        index = lastNode->numOfTuples;

        resultRow1 = 1;
        resultRow2 = 1;
        resultAppend(res, resultRow1, resultRow2);

        if ((i == 0) || (i == (res->maxTuples / 2)) || (i == (res->maxTuples - 100)) || (i == ((res->maxTuples)*2) - 1)) {

            CU_ASSERT(lastNode->buffer[index].rowId1 == resultRow1);
            CU_ASSERT(lastNode->buffer[index].rowId2 == resultRow2);

        }
    }

    resultDestroy(res);
}


void testResultGetTotalTuples(void) {

    ResultHead *res;
    resultInit(&res);

    CU_ASSERT(resultGetTotalTuples(res) == 0);

    resultAppend(res,1,2);

    CU_ASSERT(resultGetTotalTuples(res) == 1);

    for (int i = 0; i < 123; ++i) {
        resultAppend(res,i,i);
    }

    CU_ASSERT(resultGetTotalTuples(res) == 124);

    resultDestroy(res);
}


void testResultRestart(void) {

    ResultHead *res;
    resultInit(&res);

    resultAppend(res,1,2);

    resultRestart(res);

    CU_ASSERT(resultGetTotalTuples(res) == 0);

    for (int i = 0; i < 123; ++i) {
        resultAppend(res,i,i);
    }

    resultRestart(res);

    CU_ASSERT(resultGetTotalTuples(res) == 0);

    resultDestroy(res);
}


void testIntermediateResultInit(void) {

    IntermediateResultHead *iRes;
    intermediateResultInit(&iRes);

    CU_ASSERT_PTR_NOT_NULL(iRes);
    CU_ASSERT_PTR_NULL(iRes->last);
    CU_ASSERT_PTR_NULL(iRes->first);

    intermediateResultDestroy(iRes);
}


void testIntermediateResultCreateNewNode(void) {

    IntermediateResultHead *iRes;
    intermediateResultInit(&iRes);

    intermediateResultCreateNewNode(iRes,1,5);

    CU_ASSERT(iRes->first == iRes->last);

    CU_ASSERT(iRes->first->relationId == 1);
    CU_ASSERT(iRes->first->numRowIds == 5);
    CU_ASSERT(iRes->first->numJoinedRels == 0);
    CU_ASSERT_PTR_NULL(iRes->first->joinedRels);
    CU_ASSERT_PTR_NOT_NULL(iRes->first->rowIds);
    CU_ASSERT_PTR_NULL(iRes->first->next);

    intermediateResultCreateNewNode(iRes,0,10);

    CU_ASSERT(iRes->first != iRes->last);
    CU_ASSERT(iRes->first->next == iRes->last);

    CU_ASSERT(iRes->last->relationId == 0);
    CU_ASSERT(iRes->last->numRowIds == 10);
    CU_ASSERT(iRes->last->numJoinedRels == 0);
    CU_ASSERT_PTR_NULL(iRes->last->joinedRels);
    CU_ASSERT_PTR_NOT_NULL(iRes->last->rowIds);
    CU_ASSERT_PTR_NULL(iRes->last->next);

    intermediateResultCreateNewNode(iRes,2,20);

    CU_ASSERT(iRes->first != iRes->last);
    CU_ASSERT(iRes->first->next->next == iRes->last);

    CU_ASSERT(iRes->last->relationId == 2);
    CU_ASSERT(iRes->last->numRowIds == 20);
    CU_ASSERT(iRes->last->numJoinedRels == 0);
    CU_ASSERT_PTR_NULL(iRes->last->joinedRels);
    CU_ASSERT_PTR_NOT_NULL(iRes->last->rowIds);
    CU_ASSERT_PTR_NULL(iRes->last->next);


    intermediateResultDestroy(iRes);
}


void testIntermediateResultAddJoinedRel(void) {

    IntermediateResultHead *iRes;
    intermediateResultInit(&iRes);

    intermediateResultCreateNewNode(iRes,1,5);
    intermediateResultCreateNewNode(iRes,0,10);
    intermediateResultCreateNewNode(iRes,2,20);

    intermediateResultAddJoinedRel(iRes->first, iRes->last);

    CU_ASSERT(iRes->first->numJoinedRels == 1);
    CU_ASSERT(iRes->first->joinedRels[0] == iRes->last);

    intermediateResultAddJoinedRel(iRes->first, iRes->first->next);

    CU_ASSERT(iRes->first->numJoinedRels == 2);
    CU_ASSERT(iRes->first->joinedRels[0] == iRes->last);
    CU_ASSERT(iRes->first->joinedRels[1] == iRes->first->next);

    intermediateResultDestroy(iRes);
}


void testIntermediateResultIsContained(void) {

    IntermediateResultHead *iRes;
    intermediateResultInit(&iRes);

    CU_ASSERT(intermediateResultIsContained(iRes,0) == 0);
    CU_ASSERT(intermediateResultIsContained(iRes,1) == 0);
    CU_ASSERT(intermediateResultIsContained(iRes,2) == 0);

    intermediateResultCreateNewNode(iRes,1,5);

    CU_ASSERT(intermediateResultIsContained(iRes,1) == 1);

    intermediateResultCreateNewNode(iRes,0,10);

    CU_ASSERT(intermediateResultIsContained(iRes,0) == 1);

    intermediateResultCreateNewNode(iRes,2,20);

    CU_ASSERT(intermediateResultIsContained(iRes,0) == 1);
    CU_ASSERT(intermediateResultIsContained(iRes,1) == 1);
    CU_ASSERT(intermediateResultIsContained(iRes,2) == 1);
    CU_ASSERT(intermediateResultIsContained(iRes,3) == 0);
    CU_ASSERT(intermediateResultIsContained(iRes,4) == 0);


    intermediateResultDestroy(iRes);
}


void testIntermediateResultGetNode(void) {

    IntermediateResultHead *iRes;
    intermediateResultInit(&iRes);

    CU_ASSERT_PTR_NULL(intermediateResultGetNode(iRes,1));

    intermediateResultCreateNewNode(iRes,1,5);

    CU_ASSERT(iRes->first == intermediateResultGetNode(iRes,1));
    CU_ASSERT_PTR_NULL(intermediateResultGetNode(iRes,0));

    intermediateResultCreateNewNode(iRes,0,10);

    CU_ASSERT(iRes->last == intermediateResultGetNode(iRes,0));
    CU_ASSERT_PTR_NULL(intermediateResultGetNode(iRes,2));

    intermediateResultCreateNewNode(iRes,2,20);

    CU_ASSERT(iRes->last == intermediateResultGetNode(iRes,2));
    CU_ASSERT(iRes->first == intermediateResultGetNode(iRes,1));
    CU_ASSERT_PTR_NULL(intermediateResultGetNode(iRes,5));

    intermediateResultDestroy(iRes);
}


void testIntermediateResultUpdateSize(void) {

    IntermediateResultHead *iRes;
    intermediateResultInit(&iRes);

    intermediateResultCreateNewNode(iRes,1,5);
    intermediateResultCreateNewNode(iRes,0,10);
    intermediateResultCreateNewNode(iRes,2,20);

    intermediateResultUpdateSize(iRes,1,10);

    CU_ASSERT(iRes->first->numRowIds == 10);

    // In this case nothing happens
    intermediateResultUpdateSize(iRes,4,20);

    intermediateResultUpdateSize(iRes,0,20);
    intermediateResultUpdateSize(iRes,2,50);

    CU_ASSERT(iRes->first->next->numRowIds == 20);
    CU_ASSERT(iRes->last->numRowIds == 50);

    intermediateResultDestroy(iRes);
}