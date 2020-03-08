//
// Created by kalo-pc on 16/1/2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../include/unit_joinEnumeration.h"
#include "../../include/joinEnumeration.h"
#include "CUnit/Basic.h"


void testExistsInS(void) {

    /*test 1 exists*/
    int *S = (int *)malloc(5 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(S);
    S[0] = 5;
    S[1] = 1;
    S[2] = 10032;
    S[3] = 322;
    S[4] = 9;
    CU_ASSERT(existsInS(5, S, 5) == 1);
    free(S);

    /*test 2 not exists*/
    S = (int *)malloc(10 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(S);
    S[0] = 5;
    S[1] = 1;
    S[2] = 10032;
    S[3] = 322;
    S[4] = 9;
    S[5] = 523;
    S[6] = 12356;
    S[7] = 102;
    S[8] = 3102;
    S[9] = 756;
    CU_ASSERT(existsInS(10, S, 10) == 0);
    free(S);

    /*test 3 exists*/
    S = (int *)malloc(15 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(S);
    S[0] = 5;
    S[1] = 1;
    S[2] = 10032;
    S[3] = 322;
    S[4] = 9;
    S[5] = 523;
    S[6] = 12356;
    S[7] = 102;
    S[8] = 3102;
    S[9] = 756;
    S[10] = 123;
    S[11] = 346;
    S[12] = 49575;
    S[13] = 2458;
    S[14] = 3489;
    CU_ASSERT(existsInS(3102, S, 15) == 1);
    free(S);

    /*test 4 exists*/
    S = (int *)malloc(1 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(S);
    S[0] = 5;
    CU_ASSERT(existsInS(5, S, 1) == 1);
    free(S);

    /*test 5 exists*/
    S = (int *)malloc(1 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(S);
    S[0] = 5;
    CU_ASSERT(existsInS(6, S, 1) == 0);
    free(S);
}


void testMergeArray(void) {

    int *S = NULL;
    int *newArray = NULL;

    /*test 1 exists*/
    S = (int *)malloc(5 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(S);
    for (int i = 0; i < 5; ++i) {
        S[i] = i;
    }
    newArray = MergeArray(S, 7, 5);
    CU_ASSERT_PTR_NOT_NULL(newArray);
    CU_ASSERT(newArray[5] == 7);
    free(S);
    free(newArray);

    /*test 2 exists*/
    S = (int *)malloc(1000 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(S);
    for (int i = 0; i < 1000; ++i) {
        S[i] = i;
    }
    newArray = MergeArray(S, 1234, 1000);
    CU_ASSERT_PTR_NOT_NULL(newArray);
    CU_ASSERT(newArray[1000] == 1234);
    free(S);
    free(newArray);

    /*test 3 exists*/
    S = NULL;
    newArray = MergeArray(S, 15, 0);
    CU_ASSERT_PTR_NOT_NULL(newArray);
    CU_ASSERT(newArray[0] == 15);
    free(newArray);
}


void testBinomialCoeff(void) {

    uint64_t n = 0, k = 0;
    int result = 0;

    /*test 1*/
    n = 4;
    k = 1;
    result = binomialCoeff(n, k);
    CU_ASSERT(result == 4);


    /*test 2*/
    n = 15;
    k = 3;
    result = binomialCoeff(n, k);
    CU_ASSERT(result == 455);


    /*test 3*/
    n = 10;
    k = 1;
    result = binomialCoeff(n, k);
    CU_ASSERT(result == 10);


    /*test 4*/
    n = 9;
    k = 5;
    result = binomialCoeff(n, k);
    CU_ASSERT(result == 126);


    /*test 6*/
    n = 5;
    k = 5;
    result = binomialCoeff(n, k);
    CU_ASSERT(result == 1);

}


void testHashFunctionPosition(void) {

    int *n = NULL;
    uint64_t result = 0;


    /*test 1*/
    n = (int *)malloc(5 * sizeof(int));
    n[0] = 5;
    n[1] = 4;
    n[2] = 9;
    n[3] = 1;
    n[4] = 0;
    result = hashFunctionPosition(n, 5);
    CU_ASSERT(result == 563);
    free(n);


    /*test 2*/
    n = (int *)malloc(1 * sizeof(int));
    n[0] = 13;
    result = hashFunctionPosition(n, 1);
    CU_ASSERT(result == 8192);
    free(n);


    /*test 3*/
    n = (int *)malloc(7 * sizeof(int));
    n[0] = 5;
    n[1] = 4;
    n[2] = 9;
    n[3] = 1;
    n[4] = 0;
    n[5] = 7;
    n[6] = 13;
    result = hashFunctionPosition(n, 7);
    CU_ASSERT(result == 8883);
    free(n);


    /*test 4*/
    n = (int *)malloc(3 * sizeof(int));
    n[0] = 1;
    n[1] = 8;
    n[2] = 11;
    result = hashFunctionPosition(n, 3);
    CU_ASSERT(result == 2306);
    free(n);


    /*test 5*/
    n = (int *)malloc(10 * sizeof(int));
    n[0] = 1;
    n[1] = 8;
    n[2] = 11;
    n[3] = 9;
    n[4] = 15;
    n[5] = 4;
    n[6] = 3;
    n[7] = 0;
    n[8] = 7;
    n[9] = 10;
    result = hashFunctionPosition(n, 10);
    CU_ASSERT(result == 36763);
    free(n);
}


void testStructCompinationInit(void) {

    int size = 0, binomialSize = 0;
    Subset_struct *subset_struct_array;

    /*test 1*/
    size = 5;
    structCompinationInit(size, &subset_struct_array);
    CU_ASSERT_PTR_NOT_NULL(subset_struct_array);
    for (int i = 0; i < size; ++i) {
        binomialSize = binomialCoeff(size, i + 1);
        CU_ASSERT(subset_struct_array[i].pos == binomialSize);
    }
    structCompinationDestroy(subset_struct_array, size);


    /*test 2*/
    size = 10;
    structCompinationInit(size, &subset_struct_array);
    CU_ASSERT_PTR_NOT_NULL(subset_struct_array);
    for (int i = 0; i < size; ++i) {
        binomialSize = binomialCoeff(size, i + 1);
        CU_ASSERT(subset_struct_array[i].pos == binomialSize);
    }
    structCompinationDestroy(subset_struct_array, size);


    /*test 3*/
    size = 3;
    structCompinationInit(size, &subset_struct_array);
    CU_ASSERT_PTR_NOT_NULL(subset_struct_array);
    for (int i = 0; i < size; ++i) {
        binomialSize = binomialCoeff(size, i + 1);
        CU_ASSERT(subset_struct_array[i].pos == binomialSize);
    }
    structCompinationDestroy(subset_struct_array, size);


    /*test 4*/
    size = 1;
    structCompinationInit(size, &subset_struct_array);
    CU_ASSERT_PTR_NOT_NULL(subset_struct_array);
    for (int i = 0; i < size; ++i) {
        binomialSize = binomialCoeff(size, i + 1);
        CU_ASSERT(subset_struct_array[i].pos == binomialSize);
    }
    structCompinationDestroy(subset_struct_array, size);
}


void testSubsetsCompinations(void) {


    Subset_struct *subset_struct_array;
    int *arr = NULL;

    /*test 1*/
    arr = (int *)malloc(5 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(arr);
    arr[0] = 5;
    arr[1] = 2;
    arr[2] = 1;
    arr[3] = 8;
    arr[4] = 0;
    structCompinationInit(5, &subset_struct_array);
    CU_ASSERT_PTR_NOT_NULL(subset_struct_array);
    getCombination(subset_struct_array, arr, 5);

    CU_ASSERT(subset_struct_array[0].subset_array[0][0] == arr[0]);
    CU_ASSERT(subset_struct_array[0].subset_array[1][0] == arr[1]);
    CU_ASSERT(subset_struct_array[0].subset_array[2][0] == arr[2]);
    CU_ASSERT(subset_struct_array[0].subset_array[3][0] == arr[3]);
    CU_ASSERT(subset_struct_array[0].subset_array[4][0] == arr[4]);

    CU_ASSERT(subset_struct_array[4].subset_array[0][0] == arr[0]);
    CU_ASSERT(subset_struct_array[4].subset_array[0][1] == arr[1]);
    CU_ASSERT(subset_struct_array[4].subset_array[0][2] == arr[2]);
    CU_ASSERT(subset_struct_array[4].subset_array[0][3] == arr[3]);
    CU_ASSERT(subset_struct_array[4].subset_array[0][4] == arr[4]);
    //printSubsetStructArray(subset_struct_array, 5);
    structCompinationDestroy(subset_struct_array, 5);
    free(arr);


    /*test 2*/
    arr = (int *)malloc(10 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(arr);
    arr[0] = 4;
    arr[1] = 1;
    arr[2] = 6;
    arr[3] = 9;
    arr[4] = 2;
    arr[5] = 0;
    arr[6] = 5;
    arr[7] = 8;
    arr[8] = 3;
    arr[9] = 7;
    structCompinationInit(10, &subset_struct_array);
    CU_ASSERT_PTR_NOT_NULL(subset_struct_array);
    getCombination(subset_struct_array, arr, 10);

    CU_ASSERT(subset_struct_array[0].subset_array[0][0] == arr[0]);
    CU_ASSERT(subset_struct_array[0].subset_array[1][0] == arr[1]);
    CU_ASSERT(subset_struct_array[0].subset_array[2][0] == arr[2]);
    CU_ASSERT(subset_struct_array[0].subset_array[3][0] == arr[3]);
    CU_ASSERT(subset_struct_array[0].subset_array[4][0] == arr[4]);
    CU_ASSERT(subset_struct_array[0].subset_array[5][0] == arr[5]);
    CU_ASSERT(subset_struct_array[0].subset_array[6][0] == arr[6]);
    CU_ASSERT(subset_struct_array[0].subset_array[7][0] == arr[7]);
    CU_ASSERT(subset_struct_array[0].subset_array[8][0] == arr[8]);
    CU_ASSERT(subset_struct_array[0].subset_array[9][0] == arr[9]);

    CU_ASSERT(subset_struct_array[9].subset_array[0][0] == arr[0]);
    CU_ASSERT(subset_struct_array[9].subset_array[0][1] == arr[1]);
    CU_ASSERT(subset_struct_array[9].subset_array[0][2] == arr[2]);
    CU_ASSERT(subset_struct_array[9].subset_array[0][3] == arr[3]);
    CU_ASSERT(subset_struct_array[9].subset_array[0][4] == arr[4]);
    CU_ASSERT(subset_struct_array[9].subset_array[0][5] == arr[5]);
    CU_ASSERT(subset_struct_array[9].subset_array[0][6] == arr[6]);
    CU_ASSERT(subset_struct_array[9].subset_array[0][7] == arr[7]);
    CU_ASSERT(subset_struct_array[9].subset_array[0][8] == arr[8]);
    CU_ASSERT(subset_struct_array[9].subset_array[0][9] == arr[9]);
    //printSubsetStructArray(subset_struct_array, 10);
    structCompinationDestroy(subset_struct_array, 10);
    free(arr);


    /*test 3*/
    arr = (int *)malloc(3 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(arr);
    arr[0] = 2;
    arr[1] = 1;
    arr[2] = 3;
    structCompinationInit(3, &subset_struct_array);
    CU_ASSERT_PTR_NOT_NULL(subset_struct_array);
    getCombination(subset_struct_array, arr, 3);

    CU_ASSERT(subset_struct_array[0].subset_array[0][0] == arr[0]);
    CU_ASSERT(subset_struct_array[0].subset_array[1][0] == arr[1]);
    CU_ASSERT(subset_struct_array[0].subset_array[2][0] == arr[2]);

    CU_ASSERT(subset_struct_array[2].subset_array[0][0] == arr[0]);
    CU_ASSERT(subset_struct_array[2].subset_array[0][1] == arr[1]);
    CU_ASSERT(subset_struct_array[2].subset_array[0][2] == arr[2]);
    //printSubsetStructArray(subset_struct_array, 3);
    structCompinationDestroy(subset_struct_array, 3);
    free(arr);


    /*test 4*/
    arr = (int *)malloc(1 * sizeof(int));
    CU_ASSERT_PTR_NOT_NULL(arr);
    arr[0] = 1;
    structCompinationInit(1, &subset_struct_array);
    CU_ASSERT_PTR_NOT_NULL(subset_struct_array);
    getCombination(subset_struct_array, arr, 1);

    CU_ASSERT(subset_struct_array[0].subset_array[0][0] == arr[0]);

    //printSubsetStructArray(subset_struct_array, 1);
    structCompinationDestroy(subset_struct_array, 1);
    free(arr);
}


void testConnected(void) {

}


void testCreateJoinTree(void) {

}