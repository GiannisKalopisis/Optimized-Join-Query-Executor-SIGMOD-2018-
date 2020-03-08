//
// Created by kalo-pc on 2/1/2019.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "../include/joinEnumeration.h"
#include "../include/predicateParser.h"
#include "../include/relations.h"
#include "../include/error_check.h"
#include "../include/stats.h"


int counter;
int *tempArray;

/*main function for join enumeration*/
void joinEnumerationFunction(ArrayList **arrayList, predicateListHead *predicateHead, int arrayListSize, int *R_array, uint64_t ***curStats, int numOfRelations){

    int biggestNum = -1;
    for (int m = 0; m < arrayListSize; ++m) {
        if (R_array[m] > biggestNum){
            biggestNum = R_array[m];
        }
    }

    int size_of_BestTree = (int)pow(2, arrayListSize);
    BestTreeStruct *BestTree = (BestTreeStruct *)malloc(size_of_BestTree * sizeof(BestTreeStruct));
    NULL_POINTER_ERROR_CHECK(BestTree, __FILE__, __func__, __LINE__);
    for (int j = 0; j < size_of_BestTree; ++j) {
        BestTree[j].joinExecQueue = NULL;
        BestTree[j].joinCost = -1;
    }
    //printf("size_of_BestTree = %d\n", size_of_BestTree);

    int i = 0;
    int pos = 0, newSpos = 0;
    int set[1];
    BestTreeStruct *currTree = NULL;


    //making BestTree for arrays with 1 node
    for (i = 0; i < arrayListSize; i++){
        set[0] = R_array[i];
        pos = hashFunctionPosition(set, 1);
        //printf("pos = %d\n", pos);
        BestTree[pos].joinCost = arrayList[R_array[i]]->statistics[0][NUM_VAL];   //all columns must have same NUM_VAL  //getOneArrayStats(i, arrayList);    //statistics
        BestTree[pos].joinExecQueue = (int *)malloc(1 * sizeof(int));
        NULL_POINTER_ERROR_CHECK(BestTree[pos].joinExecQueue, __FILE__, __func__, __LINE__);
        BestTree[pos].joinExecQueue[0] = R_array[i];
        //printf("%d\n", BestTree[pos].joinExecQueue[0]);
    }

    //making subsets
    Subset_struct *subset_struct_array;
    structCompinationInit(arrayListSize, &subset_struct_array);
    getCombination(subset_struct_array, R_array, arrayListSize);
    //printSubsetStructArray(subset_struct_array, arrayListSize);

    int *S = NULL;


    //calc BestTree
    for (i = 1; i < arrayListSize; i++) { //(i = 1; i <= arrayListSize; i++)
        //printf("\n\n-------------------------------\n");
        //printf("-------------------------------\n\n");
        for (int k = 0; k < subset_struct_array[i-1].pos; k++) {
            //printf("\n\n-------------------------------\n\n");
            S = subset_struct_array[i-1].subset_array[k];
            //printf("SSS == ");

            //for (int l = 0; l < i; ++l) {
                //printf("%d ", S[l]);
            //}
            //printf("\n");

            for (int j = 0; j < arrayListSize; j++) {
                if (existsInS(R_array[j], S, i)) {
                    //printf("continueExists\n");
                    fflush(stdout);
                    continue;
                }
                if (!connected(predicateHead, S, i, R_array[j])){
                    /*
                    for (int l = 0; l < i; ++l) {
                        printf("-->%d \n", S[l]);
                    }
                    printf("-->R_j %d\n", R_array[j]);
                    printf("continueeeee\n");
                     */
                    int *temp = MergeArray(S, R_array[j], i);
                    //BestTree[hashFunctionPosition(temp, i+1)].joinCost = -2;
                    free(temp);
                    //printf("continue1\n");
                    fflush(stdout);
                    continue;
                }
                /*
                for (int l = 0; l < i; ++l) {
                    printf("-->%d \n", S[l]);
                }
                 */
                if (BestTree[hashFunctionPosition(S, i)].joinExecQueue == NULL) {
                    int *temp = MergeArray(S, R_array[j], i);
                    BestTree[hashFunctionPosition(temp, i+1)].joinCost = -2;
                    free(temp);
                    //printf("continue2\n");
                    fflush(stdout);
                    continue;
                }
                //i == size of S
                /*
                //currTree is the new BestTree type node
                printf("-->CreateJoinTree\n");
                printf("S == ");
                for (int l = 0; l < i; ++l) {
                    printf("%d|", S[l]);
                    fflush(stdout);
                }

                printf("\n");
                if (BestTree[hashFunctionPosition(S, i)].joinExecQueue == NULL) printf("BestTree[hashFunctionPosition(S, i)].joinExecQueue is NULL\n");
                fflush(stdout);
                printf("%lu \n", hashFunctionPosition(S, i));
                printf("BestTree[hashFunctionPosition(S, i)].joinExecQueue == ");
                for (int l = 0; l < i; ++l) {
                    printf("%d|", BestTree[hashFunctionPosition(S, i)].joinExecQueue[l]);
                    fflush(stdout);
                }

                printf("\n");
                 */
                //printf("Cost exists1: %lu\n", BestTree[newSpos].joinCost);
                currTree = CreateJoinTree(BestTree[hashFunctionPosition(S, i)].joinExecQueue, i, R_array[j], arrayList, predicateHead, curStats, numOfRelations);
                //printf("Cost exists1: %lu\n", BestTree[newSpos].joinCost);
                //printf("-->After CreateJoinTree\n");

                newSpos = hashFunctionPosition(currTree->joinExecQueue, i + 1);

                /*
                for (int l = 0; l < (i+1); ++l) {
                    printf("%d ", currTree->joinExecQueue[l]);
                }
                printf("\n");
                 */

                //printf("After newSpos = hashFunctionPosition(currTree->joinExecQueue, i + 1);\n");
                //printf("Cost exists: %lu\n", BestTree[newSpos].joinCost);
                //printf("Cost curr: %lu\n", currTree->joinCost);
                if ((BestTree[newSpos].joinExecQueue == NULL) || (BestTree[newSpos].joinCost > currTree->joinCost)) {
                    //printf("inside first IF\n");
                    //printf("@@@@@newSpos = %d\n", newSpos);
                    if (BestTree[newSpos].joinExecQueue == NULL) {
                        //printf("inside second IF\n");
                        /*
                        for (int l = 0; l < i; ++l) {
                            printf("%d|", BestTree[newSpos].joinExecQueue[l]);
                        }
                        printf("\n");
                         */
                    //    free(BestTree[newSpos].joinExecQueue);
                        //printf("inside second ---IF\n");
                        BestTree[newSpos].joinExecQueue = (int *)malloc((i+1) * sizeof(int));/*realloc(BestTree[newSpos].joinExecQueue, (i+1) * sizeof(int));*/
                    }
                    BestTree[newSpos].joinCost = currTree->joinCost;
                    //printf("After BestTree[newSpos].joinCost = currTree->joinCost;\n");
                    for (int l = 0; l < (i + 1); ++l) {
                        //printf("l = %d\n", l);
                        BestTree[newSpos].joinExecQueue[l] = currTree->joinExecQueue[l];
                    }
                    //BestTree[newSpos].joinExecQueue = currTree->joinExecQueue;
                    //printf("After BestTree[newSpos].joinExecQueue = currTree->joinExecQueue;\n");
                }
                free(currTree->joinExecQueue);
                free(currTree);
            }
        }
    }

    /*
    for (int i1 = 0; i1 < size_of_BestTree; ++i1) {
        printf("Cost %d = %lu ", i1, BestTree[i1].joinCost);
        if (BestTree[i1].joinExecQueue != NULL){
            printf("and not null\n");
        }
        else{
            printf("and NULL\n");
        }
    }
     */

    newSpos = hashFunctionPosition(R_array, arrayListSize);
    for (int l = 0; l < arrayListSize; ++l) {
        //printf("--Reached here! newSpos = %d, arrayListSize = %d\n", newSpos, arrayListSize);
        //if (BestTree[newSpos].joinExecQueue == NULL) printf("SKATA\n");
        R_array[l] = BestTree[newSpos].joinExecQueue[l];
    }
    //printf("Reached here!\n");
    /*
    for (int n = 0; n < arrayListSize; ++n) {
        //("-->returning R_array[%d] = %d\n", n, R_array[n]);
    }
     */

    for (int k = 0; k < size_of_BestTree; ++k) {
        if (BestTree[k].joinExecQueue != NULL) {
            free(BestTree[k].joinExecQueue);
        }
    }
    free(BestTree);

    structCompinationDestroy(subset_struct_array, arrayListSize);
}


int existsInS(int R_j, int *S, int S_size) {

    int i = 0;
    while (i < S_size){
        if (S[i] == R_j) return 1;
        i++;
    }
    return 0;
}


int *MergeArray(int *S, int R_j, int S_size) {

    int* newArray = (int *)malloc((S_size + 1) * sizeof(int));
    NULL_POINTER_ERROR_CHECK(newArray, __FILE__, __func__, __LINE__);

    int i = 0;

    for (i = 0; i < S_size; ++i) {
        newArray[i] = S[i];
    }
    newArray[i] = R_j;

    return newArray;
}



int getOneArrayStats(int array, ArrayList *list) {

    ArrayList *currentArrayList = list;
    int stats = -1;

    while ((array >= 0) && (currentArrayList != NULL)) {
        stats = currentArrayList->statistics[0][NUM_VAL];
        currentArrayList = currentArrayList->next;
        array--;
    }
    return stats;
}


void structCompinationInit(int length, Subset_struct **subset_struct_array) {

    (*subset_struct_array) = malloc(length * sizeof(Subset_struct));
    NULL_POINTER_ERROR_CHECK((*subset_struct_array), __FILE__, __func__, __LINE__);

    int binomialSize = 0;

    for (int i = 0; i < length; ++i) {
        binomialSize = binomialCoeff(length, i + 1);
        (*subset_struct_array)[i].pos = binomialSize;
        (*subset_struct_array)[i].subset_array = malloc(binomialSize * sizeof(int *));
        NULL_POINTER_ERROR_CHECK((*subset_struct_array)[i].subset_array, __FILE__, __func__, __LINE__);
        for (int j = 0; j < binomialSize; ++j) {
            (*subset_struct_array)[i].subset_array[j] = malloc((i + 1) * sizeof(int));
            NULL_POINTER_ERROR_CHECK((*subset_struct_array)[i].subset_array[j], __FILE__, __func__, __LINE__);
        }
    }
}


// Returns value of Binomial Coefficient C(n, k)
int binomialCoeff(int n, int k) {

    // Base Cases
    if (k==0 || k==n)
        return 1;

    // Recur
    return  binomialCoeff(n-1, k-1) + binomialCoeff(n-1, k);
}



void structCompinationDestroy(Subset_struct *structArray, int size) {

    for (int i = 0; i < size; i++){
        for (int j = 0; j < structArray[i].pos; j++) {
            free(structArray[i].subset_array[j]);
        }
        free(structArray[i].subset_array);
    }
    free(structArray);
}



// The main function that prints all combinations of
// size r in arr[] of size n. This function mainly
// uses combinationUtil()
void getCombination(Subset_struct *subset_struct_array, int arr[], int arr_size) {
    // A temporary array to store all combination


    counter = 0;
    for (int i = 0; i < arr_size; i++) {
        counter += binomialCoeff(arr_size, i + 1) * (i + 1);
    }

    tempArray = (int *)malloc(counter * sizeof(int));
    NULL_POINTER_ERROR_CHECK(tempArray, __FILE__, __func__, __LINE__);

    counter = 0;

    for (int k = 0; k < arr_size; ++k) {
        int data[k + 1];
        combinationUtil(arr, arr_size, k + 1, 0, data, 0);
    }

    counter = 0;

    for (int k = 0; k < arr_size; k++) {
        for (int i = 0; i < subset_struct_array[k].pos; i++) {
            for (int j = 0; j <= k; j++) {
                if (k == arr_size) break;
                subset_struct_array[k].subset_array[i][j] = tempArray[counter];
                counter++;
            }
        }
    }

    free(tempArray);
}


void printSubsetStructArray(Subset_struct *subset_struct_array, int arr_size) {

    for (int k = 0; k < arr_size; k++) {
        for (int i = 0; i < subset_struct_array[k].pos; i++) {
            for (int j = 0; j <= k; j++) {
                if (k == arr_size) break;
                printf("%d", subset_struct_array[k].subset_array[i][j]);
                printf(" ");
            }
            printf("||");
        }
        printf("\n");
    }
}

//int size_of_compinations;
/* arr[]  ---> Input Array
   n      ---> Size of input array
   r      ---> Size of a combination to be printed
   index  ---> Current index in data[]
   data[] ---> Temporary array to store current combination
   i      ---> index of current element in arr[]     */
void combinationUtil(int arr[], int size_of_input, int k, int index, int data[], int i) {

    // Current cobination is ready, print it
    if (index == k) {
        for (int j = 0; j < k; j++){
            tempArray[counter] = data[j];
            counter++;
        }
        //subset_struct_array[size_of_compinations - 1].pos++;

        return;
    }
    // When no more elements are there to put in data[]
    if (i >= size_of_input)
        return;
    // current is included, put next at next location
    data[index] = arr[i];
    combinationUtil(arr, size_of_input, k, index + 1, data, i + 1);
    // current is excluded, replace it with next
    // (Note that i+1 is passed, but index is not
    // changed)
    combinationUtil(arr, size_of_input, k, index, data, i + 1);
}


uint64_t hashFunctionPosition(int *set, int set_size) {

    uint64_t pos = 0;

    for (int i = 0; i < set_size; ++i) {
        pos = pos | (1<<set[i]);
    }
    //printf("pos: %lu", pos);
    return pos;
}


/* 1 if Relations are connected and 0 if they are not connected*/
int connected(predicateListHead *predicateHead, int *set_S, int setLength, int R_j){

    predicateList *current = predicateHead->first;

    while (current != NULL){
        //current->array2 == -1 -->filter
        if (((current->array1 != R_j) && (current->array2 != R_j)) || current->array2 == -1) {
            current = current->next;
            continue;
        }
        if (current->array1 == R_j){
            for (int i = 0; i < setLength; ++i) {
                if (set_S[i] == current->array2) return 1;
            }
        }
        else if (current->array2 == R_j){
            for (int i = 0; i < setLength; ++i) {
                if (set_S[i] == current->array1) return 1;
            }
        }
        else {
            fprintf(stderr, "Error at function: int connected(). R_j is != and == with array1 and array2.");
            exit(EXIT_FAILURE);
        }
        current = current->next;
    }

    return 0;
}


/* 1 if there are CrossProducts and 0 if don't*/
/* might not needed */
/*
int NoCrossProducts(){


}
 */


BestTreeStruct * CreateJoinTree(int *currentTreeJoinExecQueue, int currentTreeSize, int Rj, ArrayList **arrayList,
                                predicateListHead *predicateHead, uint64_t ***curStats, int numOfRelations){

    BestTreeStruct * currNode = (BestTreeStruct *)malloc(1 * sizeof(BestTreeStruct));
    NULL_POINTER_ERROR_CHECK(currNode, __FILE__, __func__, __LINE__);

    currNode->joinExecQueue = (int *)malloc((currentTreeSize + 1) * sizeof(int));
    NULL_POINTER_ERROR_CHECK(currNode->joinExecQueue, __FILE__, __func__, __LINE__);

    for (int i = 0; i < currentTreeSize; ++i) {
        currNode->joinExecQueue[i] = currentTreeJoinExecQueue[i];
    }
    currNode->joinExecQueue[currentTreeSize] = Rj;


    //printf("-->Before statsCalculatePredForJoinEnumeration\n");
    currNode->joinCost = statsCalculatePredForJoinEnumeration(arrayList, currNode->joinExecQueue, predicateHead, curStats, currentTreeSize+1, numOfRelations);
    //printf("-->After statsCalculatePredForJoinEnumeration\n");

    return currNode;
}


/*get the cost of given tree*/
/*inline*/ int costHashStruct(BestTreeStruct *BestTree, int *set, int size){
    return BestTree[hashFunctionPosition(set, size)].joinCost;
}


/*get the cost of given int array*/
/*inline*/ int costIntArray(BestTreeStruct currTree){
    return currTree.joinCost;
}