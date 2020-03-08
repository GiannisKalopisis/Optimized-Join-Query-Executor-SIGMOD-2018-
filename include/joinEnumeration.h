//
// Created by kalo-pc on 2/1/2019.
//

#ifndef RADIX_HASH_JOIN_PROJECT1_JOINENUMERATION_H
#define RADIX_HASH_JOIN_PROJECT1_JOINENUMERATION_H


#include "../include/predicateParser.h"
#include "../include/relations.h"


typedef struct BestTreeStruct{
    int joinCost;
    int *joinExecQueue;
} BestTreeStruct;


typedef struct Subset_struct {
    int pos;
    int **subset_array;
} Subset_struct;


void joinEnumerationFunction(ArrayList **arrayList, predicateListHead *predicateHead, int arrayListSize, int *R_array, uint64_t ***curStats, int numOfRelations);

void structCompinationInit(int length, Subset_struct **subset_struct_array);

int binomialCoeff(int n, int k);

void structCompinationDestroy(Subset_struct *structArray, int size);

int *MergeArray(int *S, int R_j, int S_size);

void combinationUtil(int arr[], int size_of_input, int k, int index, int data[], int i);

void getCombination(Subset_struct *subset_struct_array, int arr[], int arr_size);

void printSubsetStructArray(Subset_struct *subset_struct_array, int arr_size);

uint64_t hashFunctionPosition(int *set, int set_size);

int connected(predicateListHead *predicateHead, int *set_S, int setLength, int R_j);

int NoCrossProducts();

BestTreeStruct * CreateJoinTree(int *currentTreeJoinExecQueue, int currentTreeSize, int Rj, ArrayList **arrayList, predicateListHead *predicateHead, uint64_t ***curStats, int numOfRelations);

/*inline*/ int costHashStruct(BestTreeStruct *BestTree, int *set, int size);

/*inline*/ int costIntArray(BestTreeStruct currTree);

int existsInS(int R_j, int *S, int S_size);

int getOneArrayStats(int array, ArrayList *list);

#endif //RADIX_HASH_JOIN_PROJECT1_JOINENUMERATION_H
