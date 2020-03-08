//
// Created by kalo-pc on 23/11/2018.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/predicateParser.h"
#include "../include/error_check.h"
#include "../include/general_functions.h"
#include "../include/relations.h"
#include "../include/joinEnumeration.h"


//create head node of predicateList
void predicateListHeadInit(predicateListHead **head){

    (*head) = (predicateListHead *) malloc(sizeof(predicateListHead));
    NULL_POINTER_ERROR_CHECK((*head), __FILE__, __func__, __LINE__);

    (*head)->first = NULL;
    (*head)->last = NULL;
    (*head)->numOfPredicates = 0;
}


//analyze query and insert node for every predicate
void predicateListCreation(predicateListHead *head, char *predicates){

    char *token = NULL;

    //printf("predicates == '%s'\n", predicates);

    token = strtok(predicates, "&");
    //printf("--->token == %s\n", token);

    while (token != NULL){
        //newToken = token;
        //predicateListInsert(head, newToken, iResHead);
        predicateListNodeCreation(head, token);
        token = strtok(NULL, "&");
        //printf("--->token == %s\n", token);
    }

    predicateList *current = head->first;

    while(current != NULL){
        predicateListInsert(current, head);
        current = current->next;
    }
}


//analyze predicate and put correct values at predicateList node
//we assume that if it is filter, array is at first part of predicate
//for example: 0.1>/</=3000 correct
//             3000>/</=0.1 false
void predicateListInsert(predicateList *currentNode, predicateListHead *head){

    char *tmp = (char *) malloc((strlen(currentNode->predicate) + 1)* sizeof(char));
    NULL_POINTER_ERROR_CHECK(tmp, __FILE__, __func__, __LINE__);
    strcpy(tmp, currentNode->predicate);

    int dotNum = getDots(currentNode->predicate);

    //if it has 2 dots that means predicate is join because filters use only integers
    if(dotNum == 2){
        currentNode->isFilter = 0;
    }
    else {
        if (strstr(tmp, ">") != NULL) {
            currentNode->isFilter = 1;
            // Remove this
            currentNode->priority = 5;
            // Less than / Greater than filter
            currentNode->predType = FILTER_LT_GT;
        }
        else if (strstr(tmp, "<") != NULL) {
            currentNode->isFilter = 2;
            // Remove this
            currentNode->priority = 5;
            // Less than / Greater than filter
            currentNode->predType = FILTER_LT_GT;
        }
        if (strstr(tmp, "=") != NULL) {
            currentNode->isFilter = 3;
            // Remove this
            currentNode->priority = 8;
            // Equality filter
            currentNode->predType = FILTER_EQ;
        }
    }

    //put first array at struct
    char *token = strtok(tmp, ".");
    currentNode->array1 = atoi(token);

    token = strtok(NULL, "=><");
    currentNode->columnOfArray1 = atoi(token);


    //if it is not filter put new array at struct
    //else put filter value
    if (currentNode->isFilter == 0){
        token = strtok(NULL, ".");
        currentNode->array2 = atoi(token);

        token = strtok(NULL, ".");
        currentNode->columnOfArray2 = atoi(token);

    }
    else {
        token = strtok(NULL, ".");
        currentNode->filterValue = atoi(token);
    }

    if (currentNode->array1 == currentNode->array2) {
        // Join of same array and column
        //currentNode->isFilter = 4;
        if (currentNode->columnOfArray1 == currentNode->columnOfArray2) {
            currentNode->predType = JOIN_SA;
            currentNode->priority = 0;
        }
        else {
            // Same array, different columns filter
            currentNode->predType = FILTER_SA;
        }
    }
    else if (currentNode->array2 != -1) {
        // Join of different arrays
        currentNode->predType = JOIN_DA;
    }

    /*
    if (currentNode->array1 != currentNode->array2) {
        predicateList *current = head->first;
        int currArray1 = currentNode->array1;
        int currArray2 = currentNode->array2;
        while (current != NULL) {
            if (((currArray1 == current->array1) || (currArray1 == current->array2)) &&
                ((currArray2 == current->array1) || (currArray2 == current->array2))) {
                currentNode->isFilter = 0;
                break;
            }
            current = current->next;
        }
    }
    */


    free(tmp);
}


//create and initialize node of predicateList
void predicateListNodeCreation(predicateListHead *head, char *predicate){

    predicateList *newNode = (predicateList *) malloc(sizeof(predicateList));
    NULL_POINTER_ERROR_CHECK(newNode, __FILE__, __func__, __LINE__);

    newNode->predicate = (char *) malloc((strlen(predicate) + 1)* sizeof(char));
    NULL_POINTER_ERROR_CHECK(newNode->predicate, __FILE__, __func__, __LINE__);
    strcpy(newNode->predicate, predicate);

    newNode->next = NULL;

    newNode->priority = -10;

    newNode->isFilter = -1;
    newNode->filterValue = 0;

    newNode->array1 = -1;
    newNode->columnOfArray1 = -1;
    newNode->Fc_1 = -1;

    newNode->array2 = -1;
    newNode->columnOfArray2 = -1;
    newNode->Fc_2 = -1;

    if (head->first == NULL){
        head->first = newNode;
    }

    if (head->last != NULL){
        head->last->next = newNode;
    }
    head->last = newNode;
    head->numOfPredicates++;
}


void DestroyPredicateList(predicateListHead *head){

    predicateList *current = head->first;
    predicateList *next = NULL;

    while (current != NULL){
        next = current->next;
        free(current->predicate);
        free(current);
        current = next;
    }

    free(head);
}



void startingPriorities(predicateListHead *predHead, ArrayHead *arrayHead) {

    predicateList *currentPredicateList = predHead->first;
    //int tempCounter = 0;

    while (currentPredicateList->next != NULL) {

        if (currentPredicateList->isFilter == 3) {
            currentPredicateList->priority = predHead->numOfPredicates;
        }

        if (currentPredicateList->isFilter == 1 || currentPredicateList->isFilter == 2) {
            currentPredicateList->priority = predHead->numOfPredicates - 1;
        }

        if (currentPredicateList->isFilter == 4) {
            currentPredicateList->priority = predHead->numOfPredicates - 2;
        }

        if (currentPredicateList->isFilter == 5) {
            currentPredicateList->priority = predHead->numOfPredicates - 3;
        }

        if (currentPredicateList->isFilter == 0) {
            currentPredicateList->priority = predHead->numOfPredicates - 4;
        }

        currentPredicateList = currentPredicateList->next;
    }
}


/*need to be fixed*/
void updatePriority(predicateListHead *head, int array1, int array2) {

    predicateList *currentNode = head->first;
    while (currentNode != NULL) {
        if (currentNode->priority != -1) {
            //it is filter
            if (array2 == -1) {
                if (currentNode->array2 == -1) {
                    if (currentNode->array1 == array1) {    
                        if (currentNode->priority < 7) {
                            currentNode->priority++;
                        }
                        else if (currentNode->priority >= 8) {
                            currentNode->priority++; 
                        }
                    }
                }
                else{
                    if ((currentNode->array1 == array1) && (currentNode->priority < 4)){
                        currentNode->priority++;
                    }
                    if ((currentNode->array2 == array1) && (currentNode->priority < 4)){
                        currentNode->priority++;
                    }
                }
            }
            //it is conjuction
            else {
                if (currentNode->array2 == -1) {
                    if ((currentNode->array1 == array1) || (currentNode->array1 == array2)) {    
                        if (currentNode->priority < 7) {
                            currentNode->priority++;
                        }
                        else if (currentNode->priority >= 8) {
                            currentNode->priority++; 
                        }
                    }
                }
                else{
                    if ((currentNode->array1 == array1) && (currentNode->priority < 4)){
                        currentNode->priority++;
                    }
                    if ((currentNode->array1 == array2) && (currentNode->priority < 4)){
                        currentNode->priority++;
                    }
                    if ((currentNode->array2 == array1) && (currentNode->priority < 4)){
                        currentNode->priority++;
                    }
                    if ((currentNode->array2 == array2) && (currentNode->priority < 4)){
                        currentNode->priority++;
                    }
                }
            }    
        }
        currentNode = currentNode->next;
    }
}

void predicateListCalculatePriorities(predicateListHead *head, int *R_array, int R_array_size) {

    //int joinPredicatesNum = 0;
    predicateList *currentNode = head->first;

    while (currentNode != NULL) {
        if (currentNode->predType == FILTER_EQ) {
            currentNode->priority = 2*head->numOfPredicates + 4;
        }
        else if (currentNode->predType == FILTER_LT_GT) {
            currentNode->priority = 2*head->numOfPredicates + 3;
        }
        else if (currentNode->predType == FILTER_SA) {
            currentNode->priority = 2*head->numOfPredicates + 2;
        }
        else if (currentNode->predType == JOIN_SA) {
            currentNode->priority = 2*head->numOfPredicates + 1;
        }
        currentNode = currentNode->next;
    }


    for (int i = 0; i < R_array_size; ++i) {
        currentNode = head->first;
        while (currentNode != NULL) {
            if (currentNode->predType == JOIN_DA) {
                if (currentNode->array1 == R_array[i]) {
                    for (int j = 0; j < i; ++j) {
                        if (currentNode->array2 == R_array[j]){
                            currentNode->priority = R_array_size - i;
                            break;
                        }
                    }
                }
                else if (currentNode->array2 == R_array[i]){
                    for (int j = 0; j < i; ++j) {
                        if (currentNode->array1 == R_array[j]){
                            currentNode->priority = R_array_size - i;
                            break;
                        }
                    }
                }
            }
            currentNode = currentNode->next;
        }
    }

}

/*need to be fixed*/
predicateList *getHighestPriorityPredicate(predicateListHead *head) {

    // Larger number equals higher priority, -1 means done
    predicateList *returnNode = head->first;
    predicateList *currentNode = head->first;
    int maxPriority = -1;

    while (currentNode != NULL) {
        if (currentNode->priority > maxPriority) {
            maxPriority = currentNode->priority;
            returnNode = currentNode;
        }
        currentNode = currentNode->next;
    }

    if (returnNode->priority == -1) {
        return NULL;
    }


    returnNode->priority = -1;
    return returnNode;
}
