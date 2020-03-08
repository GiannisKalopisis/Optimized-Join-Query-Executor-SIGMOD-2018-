#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "../../include/unit_relations.h"
#include "../../include/relations.h"
#include "CUnit/Basic.h"


void testrInit(void) {

    Relation *rel = NULL;
    rInit(&rel, 0);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rDestroy(rel);

    rel = NULL;
    rInit(&rel, 1000);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rDestroy(rel);

    rel = NULL;
    rInit(&rel, 1000000);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rDestroy(rel);

}


void testrNewInsert() {

    Relation *rel;

    uint64_t column[10] = {2,3,4,5,6,7,8,9,10,11};
    uint64_t rowIds[5] = {0,2,4,6,8};

    //// Test 1 - no intermediate results ////

    rInit(&rel,10);

    rNewInsert(rel, NULL, 10, column);

    for (int i = 0; i < 10; ++i) {
        CU_ASSERT(rel->tuples[i].payload == i + 2);
        CU_ASSERT(rel->tuples[i].rowId == i);
    }

    rDestroy(rel);

    //// Test 2 - with intermediate results ////

    rInit(&rel,5);

    rNewInsert(rel, rowIds, 5, column);

    for (int i = 0; i < 5; ++i) {
        CU_ASSERT(rel->tuples[i].payload == (2 * i) + 2 );
        CU_ASSERT(rel->tuples[i].rowId == i );
    }

    rDestroy(rel);
}


void testrInsert(void){

    //uint64_t array[5][5] = {{0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}, {10, 11, 12, 13, 14}, {15, 16, 17, 18, 19}, {20, 21, 22, 23, 24}};
    uint64_t **array;
    array = (uint64_t **)malloc(5 * sizeof(uint64_t *));
    CU_ASSERT_PTR_NOT_NULL(array);
    for (int j = 0; j < 5; ++j) {
        array[j] = (uint64_t *)malloc(5 * sizeof(uint64_t));
        CU_ASSERT_PTR_NOT_NULL(array[j]);
    }
    for (int k = 0; k < 5; ++k) {
        for (int i = 0; i < 5; ++i) {
            array[k][i] = k*5 + i;
        }
    }
    Relation *rel = NULL;
    rInit(&rel, 5);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rInsert(rel, array, 1, 5);
    for (int i = 0; i < 5; ++i) {
        CU_ASSERT(rel->tuples[i].payload == (i + 5));
    }
    rDestroy(rel);
    rel = NULL;
    rInit(&rel, 5);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rInsert(rel, array, 4, 5);
    for (int i = 0; i < 5; ++i) {
        CU_ASSERT(rel->tuples[i].payload == (i + 20));
    }
    rDestroy(rel);
    for (int l = 0; l < 5; ++l) {
        free(array[l]);
    }
    free(array);


    uint64_t **array1;//[3][2] = {{0, 1}, {2, 3}, {4, 5}};
    array1 = (uint64_t **)malloc(3 * sizeof(uint64_t *));
    CU_ASSERT_PTR_NOT_NULL(array1);
    for (int j = 0; j < 3; ++j) {
        array1[j] = (uint64_t *)malloc(2 * sizeof(uint64_t));
        CU_ASSERT_PTR_NOT_NULL(array1[j]);
    }
    for (int k = 0; k < 3; ++k) {
        for (int i = 0; i < 2; ++i) {
            array1[k][i] = k*2 + i;
        }
    }
    rel = NULL;
    rInit(&rel, 2);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rInsert(rel, array1, 0, 2);
    for (int i = 0; i < 2; ++i) {
        CU_ASSERT(rel->tuples[i].payload == i);
    }
    rDestroy(rel);
    rel = NULL;
    rInit(&rel, 2);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rInsert(rel, array1, 2, 2);
    for (int i = 0; i < 2; ++i) {
        CU_ASSERT(rel->tuples[i].payload == (i + 4));
    }
    rDestroy(rel);
    for (int m = 0; m < 3; ++m) {
        free(array1[m]);
    }
    free(array1);


    uint64_t **array2;//[1][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    array2 = (uint64_t **)malloc(1 * sizeof(uint64_t *));
    CU_ASSERT_PTR_NOT_NULL(array2);
    for (int j = 0; j < 1; ++j) {
        array2[j] = (uint64_t *)malloc(10 * sizeof(uint64_t));
        CU_ASSERT_PTR_NOT_NULL(array2[j]);
    }
    for (int k = 0; k < 1; ++k) {
        for (int i = 0; i < 10; ++i) {
            array2[k][i] = k*10 + i;
        }
    }
    rel = NULL;
    rInit(&rel, 10);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rInsert(rel, array2, 0, 10);
    for (int i = 0; i < 10; ++i) {
        CU_ASSERT(rel->tuples[i].payload == i);
    }
    rDestroy(rel);
    for (int m = 0; m < 1; ++m) {
        free(array2[m]);
    }
    free(array2);


    uint64_t **array3;//[10][1] = {{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}};
    array3 = (uint64_t **)malloc(10 * sizeof(uint64_t *));
    CU_ASSERT_PTR_NOT_NULL(array3);
    for (int j = 0; j < 10; ++j) {
        array3[j] = (uint64_t *)malloc(1 * sizeof(uint64_t));
        CU_ASSERT_PTR_NOT_NULL(array3[j]);
    }
    for (int k = 0; k < 10; ++k) {
        for (int i = 0; i < 1; ++i) {
            array3[k][i] = k*1 + i;
        }
    }
    rel = NULL;
    rInit(&rel, 1);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rInsert(rel, array3, 0, 1);
    CU_ASSERT(rel->tuples[0].payload == 0);
    rDestroy(rel);

    rel = NULL;
    rInit(&rel, 1);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rInsert(rel, array3, 9, 1);
    CU_ASSERT(rel->tuples[0].payload == 9);
    rDestroy(rel);

    rel = NULL;
    rInit(&rel, 1);
    CU_ASSERT_PTR_NOT_NULL(rel);
    rInsert(rel, array3, 5, 1);
    CU_ASSERT(rel->tuples[0].payload == 5);
    rDestroy(rel);

    for (int n = 0; n < 10; ++n) {
        free(array3[n]);
    }
    free(array3);
}


void testArrayHeadInit() {

    ArrayHead *head;
    arrayHeadInit(&head);
    CU_ASSERT_PTR_NOT_NULL(head);
    CU_ASSERT_PTR_NULL(head->last);
    CU_ASSERT_PTR_NULL(head->first);
    CU_ASSERT(head->numOfNodes == 0);
    arrayListDestroy(head);
}


void testArrayListInit() {

    ArrayHead *head;

    arrayHeadInit(&head);

    arrayListInit(head, 3, 4);

    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_PTR_NOT_NULL(head->first);
    CU_ASSERT(head->first == head->last);

    CU_ASSERT(head->first->numOfRows == 3);
    CU_ASSERT(head->first->numOfColumns == 4);

    CU_ASSERT(head->numOfNodes == 1);

    CU_ASSERT_PTR_NOT_NULL(head->first->statistics[MIN_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->first->statistics[MAX_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->first->statistics[NUM_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->first->statistics[DIS_VAL]);

    arrayListInit(head, 10, 50);

    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_PTR_NOT_NULL(head->first);

    CU_ASSERT(head->last->numOfRows == 10);
    CU_ASSERT(head->last->numOfColumns == 50);

    CU_ASSERT(head->numOfNodes == 2);

    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[MIN_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[MAX_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[NUM_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[DIS_VAL]);

    arrayListInit(head, 12, 30);

    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_PTR_NOT_NULL(head->first);

    CU_ASSERT(head->last->numOfRows == 12);
    CU_ASSERT(head->last->numOfColumns == 30);

    CU_ASSERT(head->numOfNodes == 3);

    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[MIN_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[MAX_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[NUM_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[DIS_VAL]);


    arrayListDestroy(head);


    arrayHeadInit(&head);

    arrayListInit(head, 10, 50);

    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_PTR_NOT_NULL(head->first);
    CU_ASSERT(head->first == head->last);

    CU_ASSERT(head->first->numOfRows == 10);
    CU_ASSERT(head->first->numOfColumns == 50);

    CU_ASSERT(head->numOfNodes == 1);

    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[MIN_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[MAX_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[NUM_VAL]);
    CU_ASSERT_PTR_NOT_NULL(head->last->statistics[DIS_VAL]);

    arrayListDestroy(head);

}



void testArrayListInsert() {

    //// Test 1 ////

    /*
     *  FILE *fp = fopen("testBinaryData1.bin", "wb");
     *  uint64_t x1[4] = {1,2,3,4};
     *  uint64_t x2[4] = {5,6,7,8};
     *  uint64_t x3[4] = {9,10,11,12};
     *
     *  uint64_t col = 3;
     *  uint64_t row = 4;
     *
     *  fwrite(&row, sizeof(uint64_t), 1, fp);
     *  fwrite(&col, sizeof(uint64_t), 1, fp);
     *
     *  fwrite(x1, sizeof(uint64_t), 4, fp);
     *  fwrite(x2, sizeof(uint64_t), 4, fp);
     *  fwrite(x3, sizeof(uint64_t), 4, fp);
     *
     *  fclose(fp);
     */

    ArrayHead *head;
    arrayHeadInit(&head);
    arrayListInsert(head, "./test_txts/testBinaryData1.bin");

    CU_ASSERT(head->first->numOfColumns == 3);
    CU_ASSERT(head->first->numOfRows == 4);

    CU_ASSERT(head->first->numOfColumns == head->last->numOfColumns);
    CU_ASSERT(head->first->numOfRows == head->last->numOfRows);

    CU_ASSERT(head->first == head->last);
    CU_ASSERT(head->numOfNodes == 1);

    for (int i = 0; i < head->last->numOfColumns; ++i) {
        for (int j = 0; j < head->last->numOfRows; ++j) {
            CU_ASSERT(head->last->arrayOfInts[i][j] == (i*head->last->numOfRows + j + 1));
        }
    }

    CU_ASSERT(head->first->statistics[0][MIN_VAL] == 1);
    CU_ASSERT(head->first->statistics[0][MAX_VAL] == 4);
    CU_ASSERT(head->first->statistics[0][NUM_VAL] == 4);
    CU_ASSERT(head->first->statistics[0][DIS_VAL] == 4);

    CU_ASSERT(head->first->statistics[1][MIN_VAL] == 5);
    CU_ASSERT(head->first->statistics[1][MAX_VAL] == 8);
    CU_ASSERT(head->first->statistics[1][NUM_VAL] == 4);
    CU_ASSERT(head->first->statistics[1][DIS_VAL] == 4);

    CU_ASSERT(head->first->statistics[2][MIN_VAL] == 9);
    CU_ASSERT(head->first->statistics[2][MAX_VAL] == 12);
    CU_ASSERT(head->first->statistics[2][NUM_VAL] == 4);
    CU_ASSERT(head->first->statistics[2][DIS_VAL] == 4);

    arrayListDestroy(head);


    //// Test 2 ////

    /*   FILE *fp = fopen("testBinaryData2.bin", "wb");
     *   uint64_t x1[6] = {10,20,30,40,50,60};
     *   uint64_t x2[6] = {70,80,90,100,110,120};
     *
     *   uint64_t col = 2;
     *   uint64_t row = 6;
     *
     *   fwrite(&row, sizeof(uint64_t), 1, fp);
     *   fwrite(&col, sizeof(uint64_t), 1, fp);
     *
     *   fwrite(x1, sizeof(uint64_t), 6, fp);
     *   fwrite(x2, sizeof(uint64_t), 6, fp);
     *
     *   fclose(fp);
     */

    arrayHeadInit(&head);
    arrayListInsert(head, "./test_txts/testBinaryData1.bin");
    arrayListInsert(head, "./test_txts/testBinaryData2.bin");
    arrayListInsert(head, "./test_txts/testBinaryData1.bin");
    arrayListInsert(head, "./test_txts/testBinaryData2.bin");

    for (int i = 0; i < head->last->numOfColumns; ++i) {
        for (int j = 0; j < head->last->numOfRows; ++j) {
            CU_ASSERT(head->last->arrayOfInts[i][j] == (i*head->last->numOfRows + j + 1)*10);
        }
    }

    CU_ASSERT(head->last->statistics[0][MIN_VAL] == 10);
    CU_ASSERT(head->last->statistics[0][MAX_VAL] == 60);
    CU_ASSERT(head->last->statistics[0][NUM_VAL] == 6);
    CU_ASSERT(head->last->statistics[0][DIS_VAL] == 6);

    CU_ASSERT(head->last->statistics[1][MIN_VAL] == 70);
    CU_ASSERT(head->last->statistics[1][MAX_VAL] == 120);
    CU_ASSERT(head->last->statistics[1][NUM_VAL] == 6);
    CU_ASSERT(head->last->statistics[1][DIS_VAL] == 6);

    arrayListDestroy(head);


    //// Test 3 ////

    /*
     *   FILE *fp = fopen("testBinaryDataSimularValues.bin", "wb");
     *   uint64_t x1[10] = {10,20,10,30,10,40,10,50,10,60};
     *   uint64_t x2[10] = {2,2,2,2,2,2,2,2,2,2};
     *
     *   uint64_t col = 2;
     *   uint64_t row = 10;
     *
     *   fwrite(&row, sizeof(uint64_t), 1, fp);
     *   fwrite(&col, sizeof(uint64_t), 1, fp);
     *
     *   fwrite(x1, sizeof(uint64_t), 10, fp);
     *   fwrite(x2, sizeof(uint64_t), 10, fp);
     *
     *   fclose(fp);
     */


    arrayHeadInit(&head);
    arrayListInsert(head, "./test_txts/testBinaryDataSimularValues.bin");

    CU_ASSERT(head->first->numOfColumns == 2);
    CU_ASSERT(head->first->numOfRows == 10);

    CU_ASSERT(head->first->numOfColumns == head->last->numOfColumns);
    CU_ASSERT(head->first->numOfRows == head->last->numOfRows);

    CU_ASSERT(head->first == head->last);
    CU_ASSERT(head->numOfNodes == 1);

    for (int i = 0; i < head->last->numOfColumns; ++i) {
        if (i == 0) {
            for (int j = 0; j < head->last->numOfRows; ++j) {
                if (j % 2 == 0) {
                    CU_ASSERT(head->last->arrayOfInts[i][j] == 10);
                }
                else {
                    CU_ASSERT(head->last->arrayOfInts[i][j] == 20 + (10 * (j / 2)));
                }
            }
        }
        else {
            for (int j = 0; j < head->last->numOfRows; ++j) {
                CU_ASSERT(head->last->arrayOfInts[i][j] == 2);
            }
        }

    }

    CU_ASSERT(head->last->statistics[0][MIN_VAL] == 10);
    CU_ASSERT(head->last->statistics[0][MAX_VAL] == 60);
    CU_ASSERT(head->last->statistics[0][NUM_VAL] == 10);
    CU_ASSERT(head->last->statistics[0][DIS_VAL] == 6);

    CU_ASSERT(head->last->statistics[1][MIN_VAL] == 2);
    CU_ASSERT(head->last->statistics[1][MAX_VAL] == 2);
    CU_ASSERT(head->last->statistics[1][NUM_VAL] == 10);
    CU_ASSERT(head->last->statistics[1][DIS_VAL] == 1);

    arrayListDestroy(head);


    //// Test 4 ////

    /*
     *   FILE *fp = fopen("testBinaryDataEmpty.bin", "wb");
     *
     *   uint64_t col = 0;
     *   uint64_t row = 0;
     *
     *   fwrite(&row, sizeof(uint64_t), 1, fp);
     *   fwrite(&col, sizeof(uint64_t), 1, fp);
     *
     *   fclose(fp);
     */


    arrayHeadInit(&head);
    arrayListInsert(head, "./test_txts/testBinaryDataEmpty.bin");

    CU_ASSERT(head->first->numOfColumns == 0);
    CU_ASSERT(head->first->numOfRows == 0);

    CU_ASSERT(head->first->numOfColumns == head->last->numOfColumns);
    CU_ASSERT(head->first->numOfRows == head->last->numOfRows);

    CU_ASSERT(head->first == head->last);
    CU_ASSERT(head->numOfNodes == 1);

    arrayListDestroy(head);

}


void testArrayListIndex() {

    ArrayHead *head;
    arrayHeadInit(&head);
    arrayListInsert(head, "./test_txts/testBinaryData1.bin");
    arrayListInsert(head, "./test_txts/testBinaryData2.bin");
    arrayListInsert(head, "./test_txts/testBinaryData1.bin");
    arrayListInsert(head, "./test_txts/testBinaryData2.bin");

    ArrayList *current = NULL;

    current = arrayListIndex(head, 0);

    CU_ASSERT_PTR_NOT_NULL(current);
    CU_ASSERT(current->numOfColumns == 3);
    CU_ASSERT(current->numOfRows == 4);

    for (int i = 0; i < current->numOfColumns; ++i) {
        for (int j = 0; j < current->numOfRows; ++j) {
            CU_ASSERT(current->arrayOfInts[i][j] == (i*current->numOfRows + j + 1));
        }
    }


    current = arrayListIndex(head, 3);

    CU_ASSERT_PTR_NOT_NULL(current);
    CU_ASSERT(current->numOfColumns == 2);
    CU_ASSERT(current->numOfRows == 6);

    for (int i = 0; i < current->numOfColumns; ++i) {
        for (int j = 0; j < current->numOfRows; ++j) {
            CU_ASSERT(current->arrayOfInts[i][j] == (i*current->numOfRows + j + 1)*10);
        }
    }

    current = arrayListIndex(head, 4);

    CU_ASSERT_PTR_NULL(current);



    arrayListDestroy(head);
}
