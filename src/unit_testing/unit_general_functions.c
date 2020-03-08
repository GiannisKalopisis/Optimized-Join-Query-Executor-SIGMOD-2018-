#include <stdio.h>
#include <string.h>

#include "../../include/unit_general_functions.h"
#include "../../include/general_functions.h"
#include "CUnit/Basic.h"


void testGetFileSize(void) {

    int row = 0, col = 0;
    getFileSize("./test_txts/Test1.txt", &row, &col);
    CU_ASSERT(row == 5);
    CU_ASSERT(col == 5);

    getFileSize("./test_txts/Test2.txt", &row, &col);
    CU_ASSERT(row == 2);
    CU_ASSERT(col == 4);

    getFileSize("./test_txts/Test3.txt", &row, &col);
    CU_ASSERT(row == 0);
    CU_ASSERT(col == 0);

    getFileSize("./test_txts/Test4.txt", &row, &col);
    CU_ASSERT(row == 11);
    CU_ASSERT(col == 9);
}


void testArrayInit(void){

    uint64_t **array = NULL;
    arrayInit(&array, 100, 5000);
    CU_ASSERT_PTR_NOT_NULL(array);
    arrayDestroy(array, 5000);

    array = NULL;
    arrayInit(&array, 1, 1);
    CU_ASSERT_PTR_NOT_NULL(array);
    arrayDestroy(array, 1);

    array = NULL;
    arrayInit(&array, 10000, 50000);
    CU_ASSERT_PTR_NOT_NULL(array);
    arrayDestroy(array, 50000);

    array = NULL;
    arrayInit(&array, 0, 0);
    CU_ASSERT_PTR_NOT_NULL(array);
    arrayDestroy(array, 0);
}


void testReadArray(void){

    uint64_t **array = NULL;
    arrayInit(&array, 5, 5);
    CU_ASSERT_PTR_NOT_NULL(array);
    readArray(array, 5, 5, "./test_txts/Test1.txt");
    CU_ASSERT(array[0][0] == 1);
    CU_ASSERT(array[2][3] == 14);
    CU_ASSERT(array[4][4] == 25);
    arrayDestroy(array, 5);

    array = NULL;
    arrayInit(&array, 4, 2);
    CU_ASSERT_PTR_NOT_NULL(array);
    readArray(array, 4, 2, "./test_txts/Test2.txt");
    CU_ASSERT(array[0][0] == 1);
    CU_ASSERT(array[1][1] == 7);
    CU_ASSERT(array[1][3] == 9);
    arrayDestroy(array, 2);

    array = NULL;
    arrayInit(&array, 9, 11);
    CU_ASSERT_PTR_NOT_NULL(array);
    readArray(array, 9, 11, "./test_txts/Test4.txt");
    CU_ASSERT(array[0][0] == 1843854654);
    CU_ASSERT(array[7][3] == 2092922473);
    CU_ASSERT(array[10][8] == 314045265);
    arrayDestroy(array, 11);
}


void testGetSpaces(void){

    CU_ASSERT(getSpaces("1 2 3") == 2);
    CU_ASSERT(getSpaces("1 2 3 4 5 6 7 8 9") == 8);
    CU_ASSERT(getSpaces("000000000") == 0);
    CU_ASSERT(getSpaces("11 22 33 44 55 66 77") == 6);
}


void testGetAmpersand(void){

    CU_ASSERT(getAmpersand("1&2&3") == 2);
    CU_ASSERT(getAmpersand("1&2&3&4&5&6&7&8&9") == 8);
    CU_ASSERT(getAmpersand("000000000") == 0);
    CU_ASSERT(getAmpersand("11&22&33&44&55&66&77") == 6);
}


void testGetDots(void){

    CU_ASSERT(getDots("1.2.3") == 2);
    CU_ASSERT(getDots("1.2.3.4.5.6.7.8.9") == 8);
    CU_ASSERT(getDots("000000000") == 0);
    CU_ASSERT(getDots("11.22.33.44.55.66.77") == 6);
}


void testGetDigits(void){

    CU_ASSERT(getDigits(85) == 2);
    CU_ASSERT(getDigits(58745896) == 8);
    CU_ASSERT(getDigits(5) == 1);
}