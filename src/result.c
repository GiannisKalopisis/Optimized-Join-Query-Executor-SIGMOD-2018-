#include <string.h>
#include <stdlib.h>

#include "../include/result.h"
#include "../include/error_check.h"


void intermediateResultInit(IntermediateResultHead **iResHead) {

    (*iResHead) = (IntermediateResultHead *) malloc(sizeof(IntermediateResultHead));
    NULL_POINTER_ERROR_CHECK((*iResHead), __FILE__, __func__, __LINE__);

    (*iResHead)->first = NULL;
    (*iResHead)->last = NULL;

}

void intermediateResultCreateNewNode(IntermediateResultHead *iResHead, int relId, int size) {

    // First Node
    if (iResHead->last == NULL) {
        iResHead->last = (IntermediateResult *) malloc(sizeof(IntermediateResult));
        NULL_POINTER_ERROR_CHECK(iResHead->last, __FILE__, __func__, __LINE__);

        iResHead->last->relationId = relId;
        iResHead->last->numRowIds = size;
        iResHead->last->rowIds = (uint64_t *) malloc(size*sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK(iResHead->last->rowIds, __FILE__, __func__, __LINE__);
        iResHead->last->numJoinedRels = 0;
        iResHead->last->joinedRels = NULL;
        iResHead->last->next = NULL;

        iResHead->first = iResHead->last;
    }
    else {
        iResHead->last->next = (IntermediateResult *) malloc(sizeof(IntermediateResult));
        NULL_POINTER_ERROR_CHECK(iResHead->last->next, __FILE__, __func__, __LINE__);

        iResHead->last->next->relationId = relId;
        iResHead->last->next->numRowIds = size;
        iResHead->last->next->rowIds = (uint64_t *) malloc(size*sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK(iResHead->last->next->rowIds, __FILE__, __func__, __LINE__);
        iResHead->last->next->numJoinedRels = 0;
        iResHead->last->next->joinedRels = NULL;
        iResHead->last->next->next = NULL;

        iResHead->last = iResHead->last->next;
    }
}

void intermediateResultAddJoinedRel(IntermediateResult *iRes, IntermediateResult *joinRel) {

    iRes->numJoinedRels++;
    iRes->joinedRels = realloc(iRes->joinedRels, iRes->numJoinedRels*sizeof(IntermediateResult*));
    iRes->joinedRels[iRes->numJoinedRels - 1] = joinRel;
}

int intermediateResultIsContained(IntermediateResultHead *iResHead, int relId) {

    IntermediateResult *cur = iResHead->first;
    while (cur != NULL) {
        if (cur->relationId == relId) return 1;
        cur = cur->next;
    }
    return 0;
}

IntermediateResult * intermediateResultGetNode(IntermediateResultHead *iResHead, int relId) {

    IntermediateResult *cur = iResHead->first;
    while (cur != NULL) {
        if (cur->relationId == relId) return cur;
        cur = cur->next;
    }
    return NULL;
}

void intermediateResultUpdateSize(IntermediateResultHead *iResHead, int relId, int newSize) {

    IntermediateResult *cur = iResHead->first;
    while (cur != NULL) {
        if (cur->relationId == relId) {
            cur->numRowIds = newSize;
            if (newSize != 0) {
                cur->rowIds = realloc(cur->rowIds, newSize*sizeof(uint64_t));
                NULL_POINTER_ERROR_CHECK(cur->rowIds, __FILE__, __func__, __LINE__);
            }
            return;
        }
        else {
            cur = cur->next;
        }
    }
    return;
}

void intermediateResultPrintAll(IntermediateResultHead *iResHead){

    IntermediateResult *current = iResHead->first;
    int index;
    while (current != NULL){
        printf("Relation id = %d, numRowIds = %d\n",current->relationId, current->numRowIds);
        printf("-numJoinedRowIds = %d\n", current->numJoinedRels);
        for (int i = 0; i < current->numJoinedRels; ++i) {
            printf("  %d,", current->joinedRels[i]->relationId);
        }
        printf("\n");
        index = 0;
        while (index < current->numRowIds){
            //printf("--Index %d", index);
            //printf(", RowId = %lu\n", current->rowIds[index]);
            index++;
        }
        current = current->next;
        printf("\n");
    }
}


void intermediateResultDestroy(IntermediateResultHead *iResHead) {

    IntermediateResult *tmp = iResHead->first, *tmp2;
    while (tmp != NULL) {
        free(tmp->rowIds);
        if (tmp->joinedRels != NULL) {
            free(tmp->joinedRels);
        }
        tmp2 = tmp;
        tmp = tmp->next;
        free(tmp2);
    }
    free(iResHead);
}




void resultInit(ResultHead **resHead){

    (*resHead) = (ResultHead *)malloc(sizeof(ResultHead));
    NULL_POINTER_ERROR_CHECK((*resHead), __FILE__, __func__, __LINE__);

    (*resHead)->maxTuples = (BUFFERSIZE / sizeof(ResultTuple));

    //if buffer is too small to fit tuple
    if ((*resHead)->maxTuples == 0){
        printf("Error...Buffer is not big enough to fit tuple!!!");
        exit(EXIT_FAILURE);
    }

    (*resHead)->first = resultCreateNewNode();
    (*resHead)->last = (*resHead)->first;
}


Result * resultCreateNewNode(){

    Result * res = (Result *) malloc(sizeof(Result));
    NULL_POINTER_ERROR_CHECK(res, __FILE__, __func__, __LINE__);

    res->buffer = (ResultTuple *) malloc(BUFFERSIZE);
    NULL_POINTER_ERROR_CHECK(res->buffer, __FILE__, __func__, __LINE__);

    res->next = NULL;
    res->numOfTuples = 0;

    return res;
}


uint32_t resultIsLastNodeFull(ResultHead *head){

    return ((head->last->numOfTuples + 1) > head->maxTuples) ? 1 : 0;
}


void resultAppend(ResultHead *resHead, uint64_t rowId1, uint64_t rowId2){

    // Not full buffer
    Result *lastNode = resHead->last;
    if(!resultIsLastNodeFull(resHead)){
        int index = lastNode->numOfTuples;

        lastNode->buffer[index].rowId1 = rowId1;
        lastNode->buffer[index].rowId2 = rowId2;
    }
    // Full buffer, so create new node 1MB
    else {
        if (lastNode->next == NULL) {
            lastNode->next = resultCreateNewNode();
        }
        resHead->last = lastNode->next;

        lastNode->next->buffer[0].rowId1 = rowId1;
        lastNode->next->buffer[0].rowId2 = rowId2;
    }
    resHead->last->numOfTuples++;
}


int resultGetTotalTuples(ResultHead *resultHead){

    int totalTuples = 0;
    Result *current = resultHead->first;
    while (current != NULL){
        totalTuples += current->numOfTuples;
        current = current->next;
    }

    return totalTuples;
}


void resultPrintAll(ResultHead *head){

    Result *current = head->first;
    int nodeId = 0, index;
    while (current != NULL){
        printf("Node id = %d\n",nodeId);
        index = 0;
        while (index < current->numOfTuples){
            printf("--Index %d", index);
            printf(", RowId1 = %lu, RowId2 = %lu\n", current->buffer[index].rowId1, current->buffer[index].rowId2);
            index++;
        }
        current = current->next;
        nodeId++;
        printf("\n");
    }
}


void resultRestart(ResultHead *head) {

    Result *current = head->first;
    while (current != NULL) {
        current->numOfTuples = 0;
        current = current->next;
    }
    head->last = head->first;
}


void resultDestroy(ResultHead *head){

    Result *tmp = head->first, *tmp2;
    while (tmp != NULL) {
        free(tmp->buffer);
        tmp2 = tmp;
        tmp = tmp->next;
        free(tmp2);
    }
    free(head);
}