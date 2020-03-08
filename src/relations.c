#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../include/relations.h"
#include "../include/error_check.h"

void rInit(Relation **rel, uint64_t size){

    (*rel) = (Relation *) malloc(sizeof(Relation));
    NULL_POINTER_ERROR_CHECK(*rel, __FILE__, __func__, __LINE__);

    (*rel)->tuples = (Tuple *) malloc(size*sizeof(Tuple));
    NULL_POINTER_ERROR_CHECK((*rel)->tuples, __FILE__, __func__, __LINE__);

    (*rel)->numTuples = size;
}

void rInsert(Relation *rel, uint64_t **array, int row, int columns){

    for (int i = 0; i < columns; ++i) {
        rel->tuples[i].rowId = i + 1;
        rel->tuples[i].payload = array[row][i];
    }
}


void rNewInsert(Relation *rel, uint64_t *rowIds, int numRowIds, uint64_t *row) {

    // If NULL is given as rowIds, then 1st appearance of relation on predicates
    if (rowIds == NULL) {
        for (int i = 0; i < numRowIds; ++i) {
            rel->tuples[i].rowId = i;
            rel->tuples[i].payload = row[i];
        }
    }
    // We have intermediate rowId results so use them
    else {
        for (int i = 0; i < numRowIds; ++i) {
            rel->tuples[i].rowId = i;
            rel->tuples[i].payload = row[rowIds[i]];
        }
    }
}


uint64_t rGetPayloadOfIndex(Relation *myRelation, uint64_t index){

    return myRelation->tuples[index].payload;
}


void rDestroy(Relation *rel){

    free(rel->tuples);
    free(rel);
}


void arrayHeadInit(ArrayHead **head) {

    (*head) = (ArrayHead *) malloc(sizeof(ArrayHead));
    NULL_POINTER_ERROR_CHECK(*head, __FILE__, __func__, __LINE__);

    (*head)->numOfNodes = 0;
    (*head)->first = NULL;
    (*head)->last = NULL;

}


void arrayListInit(ArrayHead *head, uint64_t rows, uint64_t columns) {

    ArrayList *currentNode = (ArrayList *) malloc(sizeof(ArrayList));
    NULL_POINTER_ERROR_CHECK(currentNode, __FILE__, __func__, __LINE__);

    if (head->first == NULL){
        head->first = currentNode;
    }

    if (head->last != NULL){
        head->last->next = currentNode;
    }
    head->last = currentNode;

    head->numOfNodes++;

    currentNode->arrayOfInts = (uint64_t **) malloc(columns * sizeof(uint64_t *));
    NULL_POINTER_ERROR_CHECK(currentNode->arrayOfInts, __FILE__, __func__, __LINE__);

    for (int i = 0; i < columns; ++i) {
        currentNode->arrayOfInts[i] = (uint64_t *) malloc(rows * sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK(currentNode->arrayOfInts[i], __FILE__, __func__, __LINE__);
    }

    currentNode->numOfRows = rows;
    currentNode->numOfColumns = columns;

    currentNode->distinctValuesArr = malloc(columns * sizeof(char*));
    NULL_POINTER_ERROR_CHECK(currentNode->distinctValuesArr, __FILE__, __func__, __LINE__);
    currentNode->statistics = malloc(columns * sizeof(uint64_t*));
    NULL_POINTER_ERROR_CHECK(currentNode->statistics, __FILE__, __func__, __LINE__);

    for (int j = 0; j < columns; ++j) {
        currentNode->statistics[j] = malloc(4 * sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK(currentNode->statistics[j], __FILE__, __func__, __LINE__);
        currentNode->distinctValuesArr[j] = NULL;
    }


    currentNode->next = NULL;
}


void arrayListInsert(ArrayHead *head, char *filename) {

    FILE *fp = fopen(filename, "rb");
    NULL_POINTER_ERROR_CHECK(fp, __FILE__, __func__, __LINE__);

    int i = 0, j = 0;

    uint64_t slots, value;
    uint64_t rows = 0;
    uint64_t columns = 0;

    fread(&rows, sizeof(uint64_t), 1, fp);
    fread(&columns, sizeof(uint64_t), 1, fp);

    arrayListInit(head, rows, columns);

    ArrayList *currentNode = head->last;

    for (i = 0; i < columns; ++i) {
        if (rows != 0) {
            fread(&currentNode->arrayOfInts[i][0], sizeof(uint64_t), 1, fp);
            currentNode->statistics[i][MIN_VAL] = currentNode->arrayOfInts[i][0];
            currentNode->statistics[i][MAX_VAL] = currentNode->arrayOfInts[i][0];
        }
        for (j = 1; j < rows; ++j) {
            fread(&currentNode->arrayOfInts[i][j], sizeof(uint64_t), 1, fp);
            if (currentNode->arrayOfInts[i][j] < currentNode->statistics[i][MIN_VAL]) {
                currentNode->statistics[i][MIN_VAL] = currentNode->arrayOfInts[i][j];
            }
            else if (currentNode->arrayOfInts[i][j] > currentNode->statistics[i][MAX_VAL]) {
                currentNode->statistics[i][MAX_VAL] = currentNode->arrayOfInts[i][j];
            }
        }
        currentNode->statistics[i][NUM_VAL] = rows;

        // (Rough)Calculation of distinct values

        slots = currentNode->statistics[i][MAX_VAL] - currentNode->statistics[i][MIN_VAL] + 1;
        if (slots > N) {
            slots = N;
            currentNode->distinctValuesArr[i] = malloc(slots * sizeof(char));
            NULL_POINTER_ERROR_CHECK(currentNode->distinctValuesArr[i], __FILE__, __func__, __LINE__);

            for (j = 0; j < slots; j++) {
                currentNode->distinctValuesArr[i][j] = 0;
            }

            for (j = 0; j < rows; j++) {
                value = (currentNode->arrayOfInts[i][j] - currentNode->statistics[i][MIN_VAL]) % N;
                currentNode->distinctValuesArr[i][value] = 1;
            }
        }
        else {
            currentNode->distinctValuesArr[i] = malloc(slots * sizeof(char));
            NULL_POINTER_ERROR_CHECK(currentNode->distinctValuesArr[i], __FILE__, __func__, __LINE__);

            for (j = 0; j < slots; j++) {
                currentNode->distinctValuesArr[i][j] = 0;
            }

            for (j = 0; j < rows; j++) {
                value = currentNode->arrayOfInts[i][j] - currentNode->statistics[i][MIN_VAL];
                currentNode->distinctValuesArr[i][value] = 1;
            }
        }

        currentNode->statistics[i][DIS_VAL] = 0;
        for (j = 0; j < slots; j++) {
            if (currentNode->distinctValuesArr[i][j]) currentNode->statistics[i][DIS_VAL]++;
        }
    }

    fclose(fp);
}


ArrayList * arrayListIndex(ArrayHead *head, uint64_t index) {

    ArrayList *current = head->first;
    while (current != NULL) {
        if (index == 0) return current;
        index--;
        current = current->next;
    }
    return NULL;
}


void arrayListDestroy(ArrayHead *head){

    ArrayList *current = head->first;
    ArrayList *next = NULL;

    while (current != NULL){
        next = current->next;
        for (int i = 0; i < current->numOfColumns; ++i) {
            free(current->arrayOfInts[i]);
            free(current->statistics[i]);
            if (current->distinctValuesArr[i] != NULL ) free(current->distinctValuesArr[i]);
        }
        free(current->distinctValuesArr);
        free(current->arrayOfInts);
        free(current->statistics);

        free(current);
        current = next;
    }

    free(head);

}