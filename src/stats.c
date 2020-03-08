//
// Created by dodor on 04/01/19.
//

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "../include/stats.h"
#include "../include/relations.h"
#include "../include/predicateParser.h"
#include "../include/error_check.h"


void statsInitInitialize(ArrayList **arrayList, int numOfRelations, uint64_t ****stats) {

    (*stats) = (uint64_t ***) malloc(numOfRelations * sizeof(uint64_t **));
    NULL_POINTER_ERROR_CHECK((*stats), __FILE__, __func__, __LINE__);

    for (int i = 0; i < numOfRelations; ++i) {
        (*stats)[i] = (uint64_t **) malloc(arrayList[i]->numOfColumns * sizeof(uint64_t *));
        NULL_POINTER_ERROR_CHECK((*stats)[i], __FILE__, __func__, __LINE__);

        for (int j = 0; j < arrayList[i]->numOfColumns; ++j) {
            (*stats)[i][j] = (uint64_t *) malloc(4 * sizeof(uint64_t));
            NULL_POINTER_ERROR_CHECK((*stats)[i][j], __FILE__, __func__, __LINE__);

            (*stats)[i][j][MIN_VAL] = arrayList[i]->statistics[j][MIN_VAL];
            (*stats)[i][j][MAX_VAL] = arrayList[i]->statistics[j][MAX_VAL];
            (*stats)[i][j][NUM_VAL] = arrayList[i]->statistics[j][NUM_VAL];
            (*stats)[i][j][DIS_VAL] = arrayList[i]->statistics[j][DIS_VAL];
        }
    }
}


void statsInit(ArrayList **arrayList, int numOfRelations, uint64_t ****stats) {

    (*stats) = (uint64_t ***) malloc(numOfRelations * sizeof(uint64_t **));
    NULL_POINTER_ERROR_CHECK((*stats), __FILE__, __func__, __LINE__);

    for (int i = 0; i < numOfRelations; ++i) {
        (*stats)[i] = (uint64_t **) malloc(arrayList[i]->numOfColumns * sizeof(uint64_t *));
        NULL_POINTER_ERROR_CHECK((*stats)[i], __FILE__, __func__, __LINE__);

        for (int j = 0; j < arrayList[i]->numOfColumns; ++j) {
            (*stats)[i][j] = (uint64_t *) malloc(4 * sizeof(uint64_t));
            NULL_POINTER_ERROR_CHECK((*stats)[i][j], __FILE__, __func__, __LINE__);
        }
    }
}


void statsInitialize(ArrayList **arrayList, int numOfRelations, uint64_t ***stats){

    for (int i = 0; i < numOfRelations; ++i) {
        for (int j = 0; j < arrayList[i]->numOfColumns; ++j) {
            stats[i][j][MIN_VAL] = arrayList[i]->statistics[j][MIN_VAL];
            stats[i][j][MAX_VAL] = arrayList[i]->statistics[j][MAX_VAL];
            stats[i][j][NUM_VAL] = arrayList[i]->statistics[j][NUM_VAL];
            stats[i][j][DIS_VAL] = arrayList[i]->statistics[j][DIS_VAL];
        }
    }
}


void statsCalculatePred(ArrayList **arrayList, predicateList *curPred, uint64_t ***curStats, int numOfRelations) {

    uint64_t ***stats = NULL;
    statsInit(arrayList, numOfRelations, &stats);

    for (int i = 0; i < numOfRelations; ++i) {
        for (int j = 0; j < arrayList[i]->numOfColumns; ++j) {
            stats[i][j][MIN_VAL] = curStats[i][j][MIN_VAL];
            stats[i][j][MAX_VAL] = curStats[i][j][MAX_VAL];
            stats[i][j][NUM_VAL] = curStats[i][j][NUM_VAL];
            stats[i][j][DIS_VAL] = curStats[i][j][DIS_VAL];
        }
    }
    int k1 = 0, k2 = 0, maxI = 0, minU = 0, n = 0;

    switch (curPred->predType) {
        case FILTER_EQ:
            stats[curPred->array1][curPred->columnOfArray1][DIS_VAL] = 0;
            stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] = 0;
            // save disArray from relations and mod
            if ( arrayList[curPred->array1]->distinctValuesArr[curPred->columnOfArray1]
            [(curPred->filterValue - stats[curPred->array1][curPred->columnOfArray1][MIN_VAL] % N)]) {
                stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] = curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL] / curStats[curPred->array1][curPred->columnOfArray1][DIS_VAL];
                stats[curPred->array1][curPred->columnOfArray1][DIS_VAL] = 1;
            }
            stats[curPred->array1][curPred->columnOfArray1][MIN_VAL] = curPred->filterValue;
            stats[curPred->array1][curPred->columnOfArray1][MAX_VAL] = curPred->filterValue;

            /*
            for (int i = 0; i < arrayList[curPred->array1]->numOfRows; ++i) {
                if (arrayList[curPred->array1]->arrayOfInts[curPred->columnOfArray1][i] == curPred->filterValue) {
                    stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] = curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL] / curStats[curPred->array1][curPred->columnOfArray1][DIS_VAL];
                    stats[curPred->array1][curPred->columnOfArray1][DIS_VAL] = 1;
                    break;
                }
            }
             */

            for (int i = 0; i < arrayList[curPred->array1]->numOfColumns; ++i) {
                if (i == curPred->columnOfArray1) continue;
                //dont needed because we did initialization above
                //stats[curPred->array1][i][MIN_VAL] = curStats[curPred->array1][i][MIN_VAL];
                //stats[curPred->array1][i][MAX_VAL] = curStats[curPred->array1][i][MAX_VAL] ;
                stats[curPred->array1][i][NUM_VAL] = stats[curPred->array1][curPred->columnOfArray1][NUM_VAL];
                stats[curPred->array1][i][DIS_VAL] = curStats[curPred->array1][i][DIS_VAL] *
                        (1 -
                        pow(1 - (stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] / curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL]),
                            curStats[curPred->array1][i][NUM_VAL] / curStats[curPred->array1][i][DIS_VAL]));
            }
            break;
        case FILTER_LT_GT:
            k1 = 0, k2 = 0;
            if (curPred->isFilter == 1){        // filter: ">"
                //filter < MIN_VAL
                if (curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL] > curPred->filterValue) {
                    k1 = curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL];
                }
                //filter >= MIN_VAL
                else {
                    k1 = curPred->filterValue;
                }
                k2 = curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL];
            }
            else if (curPred->isFilter == 2){   // filter: "<"
                //filter > MAX_VAL
                if (curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL] < curPred->filterValue) {
                    k2 = curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL];
                }
                //filter <= MAX_VAL
                else {
                    k2 = curPred->filterValue;
                }
                k1 = curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL];
            }

            stats[curPred->array1][curPred->columnOfArray1][DIS_VAL] = ((k2 - k1)/
                                                                        (curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL] - curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL])) *
                                                                         curStats[curPred->array1][curPred->columnOfArray1][DIS_VAL];
            stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] = ((k2 - k1)/
                                                                        (curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL] - curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL])) *
                                                                         curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL];
            stats[curPred->array1][curPred->columnOfArray1][MIN_VAL] = k1;
            stats[curPred->array1][curPred->columnOfArray1][MAX_VAL] = k2;


            /*-----As previous-----*/
            for (int i = 0; i < arrayList[curPred->array1]->numOfColumns; ++i) {
                if (i == curPred->columnOfArray1) continue;
                //dont needed because we did initialization above
                //stats[curPred->array1][i][MIN_VAL] = curStats[curPred->array1][i][MIN_VAL];
                //stats[curPred->array1][i][MAX_VAL] = curStats[curPred->array1][i][MAX_VAL] ;
                stats[curPred->array1][i][NUM_VAL] = stats[curPred->array1][curPred->columnOfArray1][NUM_VAL];
                stats[curPred->array1][i][DIS_VAL] = curStats[curPred->array1][i][DIS_VAL] *
                                                     (1 -
                                                      pow(1 - (stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] / curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL]),
                                                          curStats[curPred->array1][i][NUM_VAL] / curStats[curPred->array1][i][DIS_VAL]));
            }
            break;
        case FILTER_SA:

            maxI = 0;
            if (curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL] > curStats[curPred->array2][curPred->columnOfArray2][MIN_VAL]){
                maxI = curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL];
            }
            else {
                maxI = curStats[curPred->array2][curPred->columnOfArray2][MIN_VAL];
            }

            minU = 0;
            if (curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL] < curStats[curPred->array2][curPred->columnOfArray2][MAX_VAL]){
                minU = curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL];
            }
            else {
                minU = curStats[curPred->array2][curPred->columnOfArray2][MAX_VAL];
            }


            /*DIS values*/
            stats[curPred->array1][curPred->columnOfArray1][DIS_VAL] = curStats[curPred->array1][curPred->columnOfArray1][DIS_VAL] *
                                                                       (1 -
                                                                        pow(1 - (stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] / curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL]),
                                                                        curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL] / curStats[curPred->array1][curPred->columnOfArray1][DIS_VAL]));
            stats[curPred->array2][curPred->columnOfArray2][DIS_VAL] = curStats[curPred->array2][curPred->columnOfArray2][DIS_VAL] *
                                                                       (1 -
                                                                        pow(1 - (stats[curPred->array2][curPred->columnOfArray2][NUM_VAL] / curStats[curPred->array2][curPred->columnOfArray2][NUM_VAL]),
                                                                        curStats[curPred->array2][curPred->columnOfArray2][NUM_VAL] / curStats[curPred->array2][curPred->columnOfArray2][DIS_VAL]));


            /*MIN value*/
            stats[curPred->array1][curPred->columnOfArray1][MIN_VAL] = maxI;
            stats[curPred->array2][curPred->columnOfArray2][MIN_VAL] = maxI;
            /*MAX value*/
            stats[curPred->array1][curPred->columnOfArray1][MAX_VAL] = minU;
            stats[curPred->array2][curPred->columnOfArray2][MAX_VAL] = minU;

            n = minU - maxI + 1;

            /*NUM of values*/
            stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] = curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL] / n;
            stats[curPred->array2][curPred->columnOfArray2][NUM_VAL] = curStats[curPred->array2][curPred->columnOfArray2][NUM_VAL] / n;



            /*-----As previous-----*/
            for (int i = 0; i < arrayList[curPred->array1]->numOfColumns; ++i) {
                if (i == curPred->columnOfArray1 || i == curPred->columnOfArray2) continue;
                //dont needed because we did initialization above
                //stats[curPred->array1][i][MIN_VAL] = curStats[curPred->array1][i][MIN_VAL];
                //stats[curPred->array1][i][MAX_VAL] = curStats[curPred->array1][i][MAX_VAL] ;
                stats[curPred->array1][i][NUM_VAL] = stats[curPred->array1][curPred->columnOfArray1][NUM_VAL];
                stats[curPred->array1][i][DIS_VAL] = curStats[curPred->array1][i][DIS_VAL] *
                                                     (1 -
                                                      pow(1 - (stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] / curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL]),
                                                          curStats[curPred->array1][i][NUM_VAL] / curStats[curPred->array1][i][DIS_VAL]));
            }
            break;
        case JOIN_DA:

            maxI = 0;
            if (curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL] > curStats[curPred->array2][curPred->columnOfArray2][MIN_VAL]){
                maxI = curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL];
            }
            else {
                maxI = curStats[curPred->array2][curPred->columnOfArray2][MIN_VAL];
            }

            minU = 0;
            if (curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL] < curStats[curPred->array2][curPred->columnOfArray2][MAX_VAL]){
                minU = curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL];
            }
            else {
                minU = curStats[curPred->array2][curPred->columnOfArray2][MAX_VAL];
            }

            /*MIN value*/
            stats[curPred->array1][curPred->columnOfArray1][MIN_VAL] = maxI;
            stats[curPred->array2][curPred->columnOfArray2][MIN_VAL] = maxI;
            /*MAX value*/
            stats[curPred->array1][curPred->columnOfArray1][MAX_VAL] = minU;
            stats[curPred->array2][curPred->columnOfArray2][MAX_VAL] = minU;

            n = minU - maxI + 1;

            /*NUM of values*/
            stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] = (curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL] *
                                                                        curStats[curPred->array2][curPred->columnOfArray2][NUM_VAL])/ n;
            stats[curPred->array2][curPred->columnOfArray2][NUM_VAL] = (curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL] *
                                                                        curStats[curPred->array2][curPred->columnOfArray2][NUM_VAL])/ n;

            /*DIS values*/
            stats[curPred->array1][curPred->columnOfArray1][DIS_VAL] = (curStats[curPred->array1][curPred->columnOfArray1][DIS_VAL] *
                                                                        curStats[curPred->array2][curPred->columnOfArray2][DIS_VAL])/ n;
            stats[curPred->array2][curPred->columnOfArray2][DIS_VAL] = (curStats[curPred->array1][curPred->columnOfArray1][DIS_VAL] *
                                                                        curStats[curPred->array2][curPred->columnOfArray2][DIS_VAL])/ n;


            for (int i = 0; i < arrayList[curPred->array1]->numOfColumns; ++i) {
                if (i == curPred->columnOfArray1) continue;
                //dont needed because we did initialization above
                //stats[curPred->array1][i][MIN_VAL] = curStats[curPred->array1][i][MIN_VAL];
                //stats[curPred->array1][i][MAX_VAL] = curStats[curPred->array1][i][MAX_VAL] ;
                stats[curPred->array1][i][NUM_VAL] = stats[curPred->array1][curPred->columnOfArray1][NUM_VAL];
                stats[curPred->array1][i][DIS_VAL] = curStats[curPred->array1][i][DIS_VAL] *
                                                     (1 -
                                                      pow(1 - (stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] / curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL]),
                                                          curStats[curPred->array1][i][NUM_VAL] / curStats[curPred->array1][i][DIS_VAL]));
            }

            for (int i = 0; i < arrayList[curPred->array2]->numOfColumns; ++i) {
                if (i == curPred->columnOfArray2) continue;
                //dont needed because we did initialization above
                //stats[curPred->array2][i][MIN_VAL] = curStats[curPred->array2][i][MIN_VAL];
                //stats[curPred->array2][i][MAX_VAL] = curStats[curPred->array2][i][MAX_VAL];
                stats[curPred->array2][i][NUM_VAL] = stats[curPred->array2][curPred->columnOfArray2][NUM_VAL];
                stats[curPred->array2][i][DIS_VAL] = curStats[curPred->array2][i][DIS_VAL] *
                                                     (1 -
                                                      pow(1 - (stats[curPred->array2][curPred->columnOfArray2][NUM_VAL] / curStats[curPred->array2][curPred->columnOfArray2][NUM_VAL]),
                                                          curStats[curPred->array2][i][NUM_VAL] / curStats[curPred->array2][i][DIS_VAL]));
            }



            break;
        case JOIN_SA:

            n = curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL] - curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL] + 1;

            //dont needed because we did initialization above
            //stats[curPred->array1][curPred->columnOfArray1][MIN_VAL] = curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL];
            //stats[curPred->array1][curPred->columnOfArray1][MAX_VAL] = curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL];
            //stats[curPred->array1][curPred->columnOfArray1][DIS_VAL] = curStats[curPred->array1][curPred->columnOfArray1][DIS_VAL];
            stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] =  (curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL] *
                                                                         curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL]) / n;

            /*-----As previous-----*/
            for (int i = 0; i < arrayList[curPred->array1]->numOfColumns; ++i) {
                if (i == curPred->columnOfArray1) continue;
                //dont needed because we did initialization above
                //stats[curPred->array1][i][MIN_VAL] = curStats[curPred->array1][i][MIN_VAL];
                //stats[curPred->array1][i][MAX_VAL] = curStats[curPred->array1][i][MAX_VAL] ;
                stats[curPred->array1][i][NUM_VAL] = stats[curPred->array1][curPred->columnOfArray1][NUM_VAL];
                //stats[curPred->array1][i][DIS_VAL] = curStats[curPred->array1][i][DIS_VAL];
            }
            break;
    }

    for (int i = 0; i < numOfRelations; ++i) {
        for (int j = 0; j < arrayList[i]->numOfColumns; ++j) {
            curStats[i][j][MIN_VAL] = stats[i][j][MIN_VAL];
            curStats[i][j][MAX_VAL] = stats[i][j][MAX_VAL];
            curStats[i][j][NUM_VAL] = stats[i][j][NUM_VAL];
            curStats[i][j][DIS_VAL] = stats[i][j][DIS_VAL];
        }
    }

    statsDestroy(arrayList, numOfRelations, stats);
}



void statsDestroy(ArrayList **arrayList, int numOfRelations, uint64_t ***stats){

    for (int i = 0; i < numOfRelations; ++i) {
        for (int j = 0; j < arrayList[i]->numOfColumns; ++j) {
            free(stats[i][j]);
        }
        free(stats[i]);
    }
    free(stats);
}


int statsCalculatePredForJoinEnumeration(ArrayList **arrayList, int *R_array, predicateListHead *predicateHead, uint64_t ***curStats, int sizeOfArray, int numOfRelations) {

    uint64_t ***stats = NULL;
    statsInit(arrayList, numOfRelations, &stats);

    for (int i = 0; i < numOfRelations; ++i) {
        for (int j = 0; j < arrayList[i]->numOfColumns; ++j) {
            stats[i][j][MIN_VAL] = curStats[i][j][MIN_VAL];
            stats[i][j][MAX_VAL] = curStats[i][j][MAX_VAL];
            stats[i][j][NUM_VAL] = curStats[i][j][NUM_VAL];
            stats[i][j][DIS_VAL] = curStats[i][j][DIS_VAL];
        }
    }

    uint64_t ***startingStats = NULL;
    statsInit(arrayList, numOfRelations, &startingStats);

    for (int i = 0; i < numOfRelations; ++i) {
        for (int j = 0; j < arrayList[i]->numOfColumns; ++j) {
            startingStats[i][j][MIN_VAL] = curStats[i][j][MIN_VAL];
            startingStats[i][j][MAX_VAL] = curStats[i][j][MAX_VAL];
            startingStats[i][j][NUM_VAL] = curStats[i][j][NUM_VAL];
            startingStats[i][j][DIS_VAL] = curStats[i][j][DIS_VAL];
        }
    }

    /*
    for (int l = 0; l < sizeOfArray; ++l) {
        printf("##Statistics_R_array[%d] = %d\n", l, R_array[l]);
    }
     */

    int maxI = 0, minU = 0, n = 0;
    int cost = 0;
    predicateList *curPred = NULL;

    int checkArray = -1;
    int checkArray2 = -1;
    int flag = 0;

    for (int k = 1; k < sizeOfArray; ++k) {

        flag = 0;
        checkArray = -1;
        checkArray2 = -1;

        curPred = predicateHead->first;
        checkArray = R_array[k];

        //printf(" Before While\n");
        while (curPred != NULL){
            if (curPred->predType == JOIN_DA) {
                if (curPred->array1 == checkArray){
                    checkArray2 = curPred->array2;
                }
                else if (curPred->array2 == checkArray){
                    checkArray2 = curPred->array1;
                }
                for (int i = 0; i < k; ++i) {
                    if (checkArray2 == R_array[i] || checkArray == R_array[i]){
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1) break;
            }
            //if (k + 1 >= sizeOfArray) break;
            //if (curPred->array1 == R_array[k] && curPred->array2 == R_array[k+1]) break;
            curPred = curPred->next;
        }
        if (curPred == NULL) {

            for (int i = 0; i < sizeOfArray; ++i) {
                printf("-->%d\n", R_array[i]);
            }
            fprintf(stderr, "Error at statsCalculatePredForJoinEnumeration().");
            exit(EXIT_FAILURE);
        }
        //printf("At stats: curPred == NULL\n");


        //printf("HEREEEE\n");

        switch (curPred->predType) {

            case JOIN_DA:

                maxI = 0;
                if (curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL] > curStats[curPred->array2][curPred->columnOfArray2][MIN_VAL]){
                    maxI = curStats[curPred->array1][curPred->columnOfArray1][MIN_VAL];
                }
                else {
                    maxI = curStats[curPred->array2][curPred->columnOfArray2][MIN_VAL];
                }

                minU = 0;
                if (curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL] < curStats[curPred->array2][curPred->columnOfArray2][MAX_VAL]){
                    minU = curStats[curPred->array1][curPred->columnOfArray1][MAX_VAL];
                }
                else {
                    minU = curStats[curPred->array2][curPred->columnOfArray2][MAX_VAL];
                }

                /*MIN value*/
                stats[curPred->array1][curPred->columnOfArray1][MIN_VAL] = maxI;
                stats[curPred->array2][curPred->columnOfArray2][MIN_VAL] = maxI;
                /*MAX value*/
                stats[curPred->array1][curPred->columnOfArray1][MAX_VAL] = minU;
                stats[curPred->array2][curPred->columnOfArray2][MAX_VAL] = minU;

                n = minU - maxI + 1;

                /*NUM of values*/
                if (n != 0) {
                    stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] =
                            (curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL] *
                             curStats[curPred->array2][curPred->columnOfArray2][NUM_VAL]) / n;
                    stats[curPred->array2][curPred->columnOfArray2][NUM_VAL] =
                            (curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL] *
                             curStats[curPred->array2][curPred->columnOfArray2][NUM_VAL]) / n;
                }
                else {
                    stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] = 0;
                    stats[curPred->array2][curPred->columnOfArray2][NUM_VAL] = 0;
                }


                /*DIS values*/
                if (n != 0) {
                    stats[curPred->array1][curPred->columnOfArray1][DIS_VAL] =
                            (curStats[curPred->array1][curPred->columnOfArray1][DIS_VAL] *
                             curStats[curPred->array2][curPred->columnOfArray2][DIS_VAL]) / n;
                    stats[curPred->array2][curPred->columnOfArray2][DIS_VAL] =
                            (curStats[curPred->array1][curPred->columnOfArray1][DIS_VAL] *
                             curStats[curPred->array2][curPred->columnOfArray2][DIS_VAL]) / n;
                }
                else {
                    stats[curPred->array1][curPred->columnOfArray1][DIS_VAL] = 0;
                    stats[curPred->array2][curPred->columnOfArray2][DIS_VAL] = 0;
                }


                for (int i = 0; i < arrayList[curPred->array1]->numOfColumns; ++i) {
                    if (i == curPred->columnOfArray1) continue;
                    //dont needed because we did initialization above
                    //stats[curPred->array1][i][MIN_VAL] = curStats[curPred->array1][i][MIN_VAL];
                    //stats[curPred->array1][i][MAX_VAL] = curStats[curPred->array1][i][MAX_VAL] ;
                    stats[curPred->array1][i][NUM_VAL] = stats[curPred->array1][curPred->columnOfArray1][NUM_VAL];
                    if (curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL] != 0 && curStats[curPred->array1][i][DIS_VAL] != 0) {
                        stats[curPred->array1][i][DIS_VAL] = curStats[curPred->array1][i][DIS_VAL] *
                                                             (1 -
                                                              pow(1 -
                                                                  (stats[curPred->array1][curPred->columnOfArray1][NUM_VAL] /
                                                                   curStats[curPred->array1][curPred->columnOfArray1][NUM_VAL]),
                                                                  curStats[curPred->array1][i][NUM_VAL] /
                                                                  curStats[curPred->array1][i][DIS_VAL]));
                    }
                    else {
                        stats[curPred->array1][i][DIS_VAL] = 0;
                    }
                }

                for (int i = 0; i < arrayList[curPred->array2]->numOfColumns; ++i) {
                    if (i == curPred->columnOfArray2) continue;
                    //dont needed because    we did initialization above
                    //stats[curPred->array2][i][MIN_VAL] = curStats[curPred->array2][i][MIN_VAL];
                    //stats[curPred->array2][i][MAX_VAL] = curStats[curPred->array2][i][MAX_VAL];
                    stats[curPred->array2][i][NUM_VAL] = stats[curPred->array2][curPred->columnOfArray2][NUM_VAL];

                    //printf("After calculation\n");

                    if (curStats[curPred->array2][curPred->columnOfArray2][NUM_VAL] != 0 && curStats[curPred->array2][i][DIS_VAL] != 0) {

                        //printf("into if\n");

                        stats[curPred->array2][i][DIS_VAL] = curStats[curPred->array2][i][DIS_VAL] *
                                                             (1 -
                                                              pow(1 -
                                                                  (stats[curPred->array2][curPred->columnOfArray2][NUM_VAL] /
                                                                   curStats[curPred->array2][curPred->columnOfArray2][NUM_VAL]),
                                                                  curStats[curPred->array2][i][NUM_VAL] /
                                                                  curStats[curPred->array2][i][DIS_VAL]));
                    }
                    else {
                        //printf("into else\n");
                        stats[curPred->array2][i][DIS_VAL] = 0;
                    }
                }



                break;
        }

        cost += stats[curPred->array1][curPred->columnOfArray1][NUM_VAL];

        for (int i = 0; i < numOfRelations; ++i) {
            for (int j = 0; j < arrayList[i]->numOfColumns; ++j) {
                curStats[i][j][MIN_VAL] = stats[i][j][MIN_VAL];
                curStats[i][j][MAX_VAL] = stats[i][j][MAX_VAL];
                curStats[i][j][NUM_VAL] = stats[i][j][NUM_VAL];
                curStats[i][j][DIS_VAL] = stats[i][j][DIS_VAL];
            }
        }
    }

    for (int i = 0; i < numOfRelations; ++i) {
        for (int j = 0; j < arrayList[i]->numOfColumns; ++j) {
            curStats[i][j][MIN_VAL] = startingStats[i][j][MIN_VAL];
            curStats[i][j][MAX_VAL] = startingStats[i][j][MAX_VAL];
            curStats[i][j][NUM_VAL] = startingStats[i][j][NUM_VAL];
            curStats[i][j][DIS_VAL] = startingStats[i][j][DIS_VAL];
        }
    }

    statsDestroy(arrayList, numOfRelations, stats);
    statsDestroy(arrayList, numOfRelations, startingStats);

    return cost;
}