//
// Created by kalo-pc on 25/11/2018.
//
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


#include "../../include/unit_predicateParser.h"
#include "../../include/predicateParser.h"
#include "CUnit/Basic.h"


void testPredicateListHeadInit(void) {

    predicateListHead *head;
    predicateListHeadInit(&head);

    CU_ASSERT_PTR_NOT_NULL(head);
    CU_ASSERT_PTR_NULL(head->first);
    CU_ASSERT_PTR_NULL(head->last);
    CU_ASSERT(head->numOfPredicates == 0);

    DestroyPredicateList(head);
}


void testPredicateListCreation(void) {

    predicateListHead *head;
    char *predicates = NULL;

    /*---test 1---*/
    /*query :: 0.1=1.2&1.1=0.1&0.1>3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1&0.1>3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1&0.1>3000");

    predicateListCreation(head, predicates);

    CU_ASSERT_PTR_NOT_NULL(head);
    CU_ASSERT(head->numOfPredicates == 3);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 2---*/
    /*query :: 0.1>3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1>3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1>3000");

    predicateListCreation(head, predicates);

    CU_ASSERT_PTR_NOT_NULL(head);
    CU_ASSERT(head->numOfPredicates == 1);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 3---*/
    /*query :: 0.1<3000&1.1=0.1&0.1=1.2&0.1<3000&1.1=0.1&0.1=1.2*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1<3000&1.1=0.1&0.1=1.2&0.1<3000&1.1=0.1&0.1=1.2") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1<3000&1.1=0.1&0.1=1.2&0.1<3000&1.1=0.1&0.1=1.2");

    predicateListCreation(head, predicates);

    CU_ASSERT_PTR_NOT_NULL(head);
    CU_ASSERT(head->numOfPredicates == 6);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 4---*/
    /*query :: 0.1=1.2&1.1=0.1&0.1=3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1&0.1=3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1&0.1=3000");

    predicateListCreation(head, predicates);

    CU_ASSERT_PTR_NOT_NULL(head);
    CU_ASSERT(head->numOfPredicates == 3);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 5---*/
    /*query :: 0.1=1.2&1.1=0.1*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1");

    predicateListCreation(head, predicates);

    CU_ASSERT_PTR_NOT_NULL(head);
    CU_ASSERT(head->numOfPredicates == 2);

    DestroyPredicateList(head);
    free(predicates);
}


void testPredicateListInsert(void) {

    predicateListHead *head;
    predicateList *current;
    char *predicates = NULL;

    /*---test 1---*/
    /*query :: 0.1=1.2&1.1=0.1&0.1>3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1&0.1>3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1&0.1>3000");

    predicateListCreation(head, predicates);

    current = head->first;

    CU_ASSERT_STRING_EQUAL(current->predicate, "0.1=1.2");
    CU_ASSERT(current->isFilter == 0);
    CU_ASSERT(current->filterValue == 0);
    CU_ASSERT(current->array1 == 0);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == 1);
    CU_ASSERT(current->columnOfArray2 == 2);
    CU_ASSERT(current->priority == -10);
    current = current->next;

    CU_ASSERT_STRING_EQUAL(current->predicate, "1.1=0.1");
    CU_ASSERT(current->isFilter == 0);
    CU_ASSERT(current->filterValue == 0);
    CU_ASSERT(current->array1 == 1);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == 0);
    CU_ASSERT(current->columnOfArray2 == 1);
    CU_ASSERT(current->priority == -10);
    current = current->next;

    CU_ASSERT_STRING_EQUAL(current->predicate, "0.1>3000");
    CU_ASSERT(current->isFilter == 1);
    CU_ASSERT(current->filterValue == 3000);
    CU_ASSERT(current->array1 == 0);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == -1);
    CU_ASSERT(current->columnOfArray2 == -1);
    CU_ASSERT(current->priority == 5);
    current = current->next;

    DestroyPredicateList(head);
    free(predicates);


    /*---test 2---*/
    /*query :: 0.1>3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1>3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1>3000");

    predicateListCreation(head, predicates);

    current = head->first;

    CU_ASSERT_STRING_EQUAL(current->predicate, "0.1>3000");
    CU_ASSERT(current->isFilter == 1);
    CU_ASSERT(current->filterValue == 3000);
    CU_ASSERT(current->array1 == 0);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == -1);
    CU_ASSERT(current->columnOfArray2 == -1);
    CU_ASSERT(current->priority == 5);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 3---*/
    /*query :: 0.1<3000&1.1=0.1&0.1=1.2&0.1<3000&1.1=0.1&0.1=1.2*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1<3000&1.1=0.1&0.1=1.2&0.2>3000&1.1=0.2&0.2=1.2") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1<3000&1.1=0.1&0.1=1.2&0.2>3000&1.1=0.2&0.2=1.2");

    predicateListCreation(head, predicates);

    current = head->first;

    CU_ASSERT_STRING_EQUAL(current->predicate, "0.1<3000");
    CU_ASSERT(current->isFilter == 2);
    CU_ASSERT(current->filterValue == 3000);
    CU_ASSERT(current->array1 == 0);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == -1);
    CU_ASSERT(current->columnOfArray2 == -1);
    CU_ASSERT(current->priority == 5);
    current = current->next;

    CU_ASSERT_STRING_EQUAL(current->predicate, "1.1=0.1");
    CU_ASSERT(current->isFilter == 0);
    CU_ASSERT(current->filterValue == 0);
    CU_ASSERT(current->array1 == 1);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == 0);
    CU_ASSERT(current->columnOfArray2 == 1);
    CU_ASSERT(current->priority == -10);
    current = current->next;


    CU_ASSERT_STRING_EQUAL(current->predicate, "0.1=1.2");
    CU_ASSERT(current->isFilter == 0);
    CU_ASSERT(current->filterValue == 0);
    CU_ASSERT(current->array1 == 0);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == 1);
    CU_ASSERT(current->columnOfArray2 == 2);
    CU_ASSERT(current->priority == -10);
    current = current->next;

    CU_ASSERT_STRING_EQUAL(current->predicate, "0.2>3000");
    CU_ASSERT(current->isFilter == 1);
    CU_ASSERT(current->filterValue == 3000);
    CU_ASSERT(current->array1 == 0);
    CU_ASSERT(current->columnOfArray1 == 2);
    CU_ASSERT(current->array2 == -1);
    CU_ASSERT(current->columnOfArray2 == -1);
    CU_ASSERT(current->priority == 5);
    current = current->next;

    CU_ASSERT_STRING_EQUAL(current->predicate, "1.1=0.2");
    CU_ASSERT(current->isFilter == 0);
    CU_ASSERT(current->filterValue == 0);
    CU_ASSERT(current->array1 == 1);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == 0);
    CU_ASSERT(current->columnOfArray2 == 2);
    CU_ASSERT(current->priority == -10);
    current = current->next;

    CU_ASSERT_STRING_EQUAL(current->predicate, "0.2=1.2");
    CU_ASSERT(current->isFilter == 0);
    CU_ASSERT(current->filterValue == 0);
    CU_ASSERT(current->array1 == 0);
    CU_ASSERT(current->columnOfArray1 == 2);
    CU_ASSERT(current->array2 == 1);
    CU_ASSERT(current->columnOfArray2 == 2);
    CU_ASSERT(current->priority == -10);
    current = current->next;

    DestroyPredicateList(head);
    free(predicates);


    /*---test 4---*/
    /*query :: 0.1=1.2&1.1=0.1&0.1=300*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1&0.1=3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1&0.1=3000");

    predicateListCreation(head, predicates);

    current = head->first;

    CU_ASSERT_STRING_EQUAL(current->predicate, "0.1=1.2");
    CU_ASSERT(current->isFilter == 0);
    CU_ASSERT(current->filterValue == 0);
    CU_ASSERT(current->array1 == 0);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == 1);
    CU_ASSERT(current->columnOfArray2 == 2);
    CU_ASSERT(current->priority == -10);
    current = current->next;

    CU_ASSERT_STRING_EQUAL(current->predicate, "1.1=0.1");
    CU_ASSERT(current->isFilter == 0);
    CU_ASSERT(current->filterValue == 0);
    CU_ASSERT(current->array1 == 1);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == 0);
    CU_ASSERT(current->columnOfArray2 == 1);
    CU_ASSERT(current->priority == -10);
    current = current->next;

    CU_ASSERT_STRING_EQUAL(current->predicate, "0.1=3000");
    CU_ASSERT(current->isFilter == 3);
    CU_ASSERT(current->filterValue == 3000);
    CU_ASSERT(current->array1 == 0);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == -1);
    CU_ASSERT(current->columnOfArray2 == -1);
    CU_ASSERT(current->priority == 8);
    current = current->next;

    DestroyPredicateList(head);
    free(predicates);


    /*---test 5---*/
    /*query :: 0.1=1.2&1.1=0.1*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1");

    predicateListCreation(head, predicates);

    current = head->first;

    CU_ASSERT_STRING_EQUAL(current->predicate, "0.1=1.2");
    CU_ASSERT(current->isFilter == 0);
    CU_ASSERT(current->filterValue == 0);
    CU_ASSERT(current->array1 == 0);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == 1);
    CU_ASSERT(current->columnOfArray2 == 2);
    CU_ASSERT(current->priority == -10);
    current = current->next;

    CU_ASSERT_STRING_EQUAL(current->predicate, "1.1=0.1");
    CU_ASSERT(current->isFilter == 0);
    CU_ASSERT(current->filterValue == 0);
    CU_ASSERT(current->array1 == 1);
    CU_ASSERT(current->columnOfArray1 == 1);
    CU_ASSERT(current->array2 == 0);
    CU_ASSERT(current->columnOfArray2 == 1);
    CU_ASSERT(current->priority == -10);
    current = current->next;

    DestroyPredicateList(head);
    free(predicates);
}


void testPredicateListNodeCreation(void) {

    predicateListHead *head;

    /*---test 1---*/
    /*query :: 0.1=1.2&1.1=0.1&0.1>3000*/
    predicateListHeadInit(&head);

    predicateListNodeCreation(head, "0.1=1.2");
    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_STRING_EQUAL(head->last->predicate, "0.1=1.2");
    CU_ASSERT(head->last->isFilter == -1);
    CU_ASSERT(head->last->filterValue == 0);
    CU_ASSERT(head->last->array1 == -1);
    CU_ASSERT(head->last->columnOfArray1 == -1);
    CU_ASSERT(head->last->array2 == -1);
    CU_ASSERT(head->last->columnOfArray2 == -1);
    CU_ASSERT(head->last->priority == -10);

    predicateListNodeCreation(head, "1.1=0.1");
    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_STRING_EQUAL(head->last->predicate, "1.1=0.1");
    CU_ASSERT(head->last->isFilter == -1);
    CU_ASSERT(head->last->filterValue == 0);
    CU_ASSERT(head->last->array1 == -1);
    CU_ASSERT(head->last->columnOfArray1 == -1);
    CU_ASSERT(head->last->array2 == -1);
    CU_ASSERT(head->last->columnOfArray2 == -1);
    CU_ASSERT(head->last->priority == -10);

    predicateListNodeCreation(head, "0.1>3000");
    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_STRING_EQUAL(head->last->predicate, "0.1>3000");
    CU_ASSERT(head->last->isFilter == -1);
    CU_ASSERT(head->last->filterValue == 0);
    CU_ASSERT(head->last->array1 == -1);
    CU_ASSERT(head->last->columnOfArray1 == -1);
    CU_ASSERT(head->last->array2 == -1);
    CU_ASSERT(head->last->columnOfArray2 == -1);
    CU_ASSERT(head->last->priority == -10);

    DestroyPredicateList(head);

    /*---test 2---*/
    /*query :: 0.1=1.2*/
    predicateListHeadInit(&head);

    predicateListNodeCreation(head, "0.1=1.2");
    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_STRING_EQUAL(head->last->predicate, "0.1=1.2");
    CU_ASSERT(head->last->isFilter == -1);
    CU_ASSERT(head->last->filterValue == 0);
    CU_ASSERT(head->last->array1 == -1);
    CU_ASSERT(head->last->columnOfArray1 == -1);
    CU_ASSERT(head->last->array2 == -1);
    CU_ASSERT(head->last->columnOfArray2 == -1);
    CU_ASSERT(head->last->priority == -10);

    DestroyPredicateList(head);

    /*---test 3---*/
    /*query :: 0.1=1.2*/
    predicateListHeadInit(&head);

    predicateListNodeCreation(head, "0.1<3000");
    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_STRING_EQUAL(head->last->predicate, "0.1<3000");
    CU_ASSERT(head->last->isFilter == -1);
    CU_ASSERT(head->last->filterValue == 0);
    CU_ASSERT(head->last->array1 == -1);
    CU_ASSERT(head->last->columnOfArray1 == -1);
    CU_ASSERT(head->last->array2 == -1);
    CU_ASSERT(head->last->columnOfArray2 == -1);
    CU_ASSERT(head->last->priority == -10);

    predicateListNodeCreation(head, "1.1=0.1");
    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_STRING_EQUAL(head->last->predicate, "1.1=0.1");
    CU_ASSERT(head->last->isFilter == -1);
    CU_ASSERT(head->last->filterValue == 0);
    CU_ASSERT(head->last->array1 == -1);
    CU_ASSERT(head->last->columnOfArray1 == -1);
    CU_ASSERT(head->last->array2 == -1);
    CU_ASSERT(head->last->columnOfArray2 == -1);
    CU_ASSERT(head->last->priority == -10);

    predicateListNodeCreation(head, "0.1=1.2");
    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_STRING_EQUAL(head->last->predicate, "0.1=1.2");
    CU_ASSERT(head->last->isFilter == -1);
    CU_ASSERT(head->last->filterValue == 0);
    CU_ASSERT(head->last->array1 == -1);
    CU_ASSERT(head->last->columnOfArray1 == -1);
    CU_ASSERT(head->last->array2 == -1);
    CU_ASSERT(head->last->columnOfArray2 == -1);
    CU_ASSERT(head->last->priority == -10);

    predicateListNodeCreation(head, "0.2>3000");
    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_STRING_EQUAL(head->last->predicate, "0.2>3000");
    CU_ASSERT(head->last->isFilter == -1);
    CU_ASSERT(head->last->filterValue == 0);
    CU_ASSERT(head->last->array1 == -1);
    CU_ASSERT(head->last->columnOfArray1 == -1);
    CU_ASSERT(head->last->array2 == -1);
    CU_ASSERT(head->last->columnOfArray2 == -1);
    CU_ASSERT(head->last->priority == -10);

    predicateListNodeCreation(head, "1.1=0.2");
    CU_ASSERT_PTR_NOT_NULL(head->last);
    CU_ASSERT_STRING_EQUAL(head->last->predicate, "1.1=0.2");
    CU_ASSERT(head->last->isFilter == -1);
    CU_ASSERT(head->last->filterValue == 0);
    CU_ASSERT(head->last->array1 == -1);
    CU_ASSERT(head->last->columnOfArray1 == -1);
    CU_ASSERT(head->last->array2 == -1);
    CU_ASSERT(head->last->columnOfArray2 == -1);
    CU_ASSERT(head->last->priority == -10);

    DestroyPredicateList(head);


}


void testUpdatePriority(void) {

    predicateListHead *head;
    char *predicates = NULL;
    predicateList *curPredicate = NULL, *testNode = NULL;


    /*---test 1---*/
    /*query :: 0.1=1.2&1.1=0.1&0.1>3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1&0.1>3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1&0.1>3000");
    predicateListCreation(head, predicates);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == 1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 3);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);

    DestroyPredicateList(head);
    free(predicates);




    /*---test 2---*/
    /*query :: 0.1>3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1>3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1>3000");
    predicateListCreation(head, predicates);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 3---*/
    /*query :: 0.1<3000&1.1=0.1&0.1=1.2&0.1<3000&1.1=0.1&0.1=1.2*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1<3000&1.1=0.1&0.1=1.2&0.1<3000&1.1=0.1&0.1=1.2") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1<3000&1.1=0.1&0.1=1.2&0.1<3000&1.1=0.1&0.1=1.2");
    predicateListCreation(head, predicates);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }

    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 6);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 1);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 2);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 2);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 2);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 2);


    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 4);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 4);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 4);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 4);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 4);


    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 4);


    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);


    DestroyPredicateList(head);
    free(predicates);


    /*---test 4---*/
    /*query :: 0.1=1.2&1.1=0.1&0.1=3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1&0.1=3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1&0.1=3000");
    predicateListCreation(head, predicates);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == 1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 3);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 5---*/
    /*query :: 0.1=1.2&1.1=0.1*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1");
    predicateListCreation(head, predicates);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == 2);

    if ((curPredicate = getHighestPriorityPredicate(head)) != NULL) {
        //printf("%p\n", curPredicate);
        if (curPredicate->isFilter) {
            updatePriority(head, curPredicate->array1, -1);
        }
            // Conjunction query
        else {
            updatePriority(head, curPredicate->array1, curPredicate->array2);
        }
    }
    testNode = head->first;
    CU_ASSERT(testNode->priority == -1);
    testNode = testNode->next;
    CU_ASSERT(testNode->priority == -1);

    DestroyPredicateList(head);
    free(predicates);
}


void testGetHighestPriorityPredicate(void) {

    predicateListHead *head;
    char *predicates = NULL;
    predicateList *listNode = NULL;

    /*---test 1---*/
    /*query :: 0.1=1.2&1.1=0.1&0.1>3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1");
    predicateListCreation(head, predicates);
    head->first->priority = 2;
    head->last->priority = 1;

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "0.1=1.2");
    CU_ASSERT(listNode->isFilter == 0);
    CU_ASSERT(listNode->filterValue == 0);
    CU_ASSERT(listNode->array1 == 0);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == 1);
    CU_ASSERT(listNode->columnOfArray2 == 2);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "1.1=0.1");
    CU_ASSERT(listNode->isFilter == 0);
    CU_ASSERT(listNode->filterValue == 0);
    CU_ASSERT(listNode->array1 == 1);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == 0);
    CU_ASSERT(listNode->columnOfArray2 == 1);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_PTR_NULL(listNode);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 2---*/
    /*query :: 0.1>3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1>3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1>3000");
    predicateListCreation(head, predicates);
    head->first->priority = 1;

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "0.1>3000");
    CU_ASSERT(listNode->isFilter == 1);
    CU_ASSERT(listNode->filterValue == 3000);
    CU_ASSERT(listNode->array1 == 0);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == -1);
    CU_ASSERT(listNode->columnOfArray2 == -1);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_PTR_NULL(listNode);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 3---*/
    /*query :: 0.1<3000&1.1=0.1&0.1=1.2&0.1<3000&1.1=0.1&0.1=1.2*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1<3000&1.1=0.1&0.1=1.2&0.1=3000&1.1=0.1&0.1=1.2") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1<3000&1.1=0.1&0.1=1.2&0.1=3000&1.1=0.1&0.1=1.2");
    predicateListCreation(head, predicates);
    head->first->priority = 5;
    head->first->next->priority = 2;
    head->first->next->next->priority = 3;
    head->first->next->next->next->priority = 6;
    head->first->next->next->next->next->priority = 4;
    head->last->priority = 1;

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "0.1=3000");
    CU_ASSERT(listNode->isFilter == 3);
    CU_ASSERT(listNode->filterValue == 3000);
    CU_ASSERT(listNode->array1 == 0);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == -1);
    CU_ASSERT(listNode->columnOfArray2 == -1);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "0.1<3000");
    CU_ASSERT(listNode->isFilter == 2);
    CU_ASSERT(listNode->filterValue == 3000);
    CU_ASSERT(listNode->array1 == 0);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == -1);
    CU_ASSERT(listNode->columnOfArray2 == -1);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "1.1=0.1");
    CU_ASSERT(listNode->isFilter == 0);
    CU_ASSERT(listNode->filterValue == 0);
    CU_ASSERT(listNode->array1 == 1);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == 0);
    CU_ASSERT(listNode->columnOfArray2 == 1);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "0.1=1.2");
    CU_ASSERT(listNode->isFilter == 0);
    CU_ASSERT(listNode->filterValue == 0);
    CU_ASSERT(listNode->array1 == 0);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == 1);
    CU_ASSERT(listNode->columnOfArray2 == 2);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "1.1=0.1");
    CU_ASSERT(listNode->isFilter == 0);
    CU_ASSERT(listNode->filterValue == 0);
    CU_ASSERT(listNode->array1 == 1);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == 0);
    CU_ASSERT(listNode->columnOfArray2 == 1);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "0.1=1.2");
    CU_ASSERT(listNode->isFilter == 0);
    CU_ASSERT(listNode->filterValue == 0);
    CU_ASSERT(listNode->array1 == 0);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == 1);
    CU_ASSERT(listNode->columnOfArray2 == 2);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_PTR_NULL(listNode);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 4---*/
    /*query :: 0.1=1.2&1.1=0.1&0.1=3000*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1=0.1&0.1=3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1=0.1&0.1=3000");
    predicateListCreation(head, predicates);
    head->first->priority = 2;
    head->first->next->priority = 1;
    head->last->priority = 3;

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "0.1=3000");
    CU_ASSERT(listNode->isFilter == 3);
    CU_ASSERT(listNode->filterValue == 3000);
    CU_ASSERT(listNode->array1 == 0);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == -1);
    CU_ASSERT(listNode->columnOfArray2 == -1);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "0.1=1.2");
    CU_ASSERT(listNode->isFilter == 0);
    CU_ASSERT(listNode->filterValue == 0);
    CU_ASSERT(listNode->array1 == 0);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == 1);
    CU_ASSERT(listNode->columnOfArray2 == 2);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "1.1=0.1");
    CU_ASSERT(listNode->isFilter == 0);
    CU_ASSERT(listNode->filterValue == 0);
    CU_ASSERT(listNode->array1 == 1);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == 0);
    CU_ASSERT(listNode->columnOfArray2 == 1);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_PTR_NULL(listNode);

    DestroyPredicateList(head);
    free(predicates);


    /*---test 5---*/
    /*query :: 0.1=1.2&1.1=0.1*/
    predicateListHeadInit(&head);

    predicates = (char *) malloc((strlen("0.1=1.2&1.1<3000&0.2>3000") + 1)* sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(predicates);
    strcpy(predicates, "0.1=1.2&1.1<3000&0.2>3000");
    predicateListCreation(head, predicates);
    head->first->priority = 1;
    head->first->next->priority = 2;
    head->last->priority = 2;

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "1.1<3000");
    CU_ASSERT(listNode->isFilter == 2);
    CU_ASSERT(listNode->filterValue == 3000);
    CU_ASSERT(listNode->array1 == 1);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == -1);
    CU_ASSERT(listNode->columnOfArray2 == -1);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "0.2>3000");
    CU_ASSERT(listNode->isFilter == 1);
    CU_ASSERT(listNode->filterValue == 3000);
    CU_ASSERT(listNode->array1 == 0);
    CU_ASSERT(listNode->columnOfArray1 == 2);
    CU_ASSERT(listNode->array2 == -1);
    CU_ASSERT(listNode->columnOfArray2 == -1);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_STRING_EQUAL(listNode->predicate, "0.1=1.2");
    CU_ASSERT(listNode->isFilter == 0);
    CU_ASSERT(listNode->filterValue == 0);
    CU_ASSERT(listNode->array1 == 0);
    CU_ASSERT(listNode->columnOfArray1 == 1);
    CU_ASSERT(listNode->array2 == 1);
    CU_ASSERT(listNode->columnOfArray2 == 2);

    listNode = getHighestPriorityPredicate(head);

    CU_ASSERT_PTR_NULL(listNode);

    DestroyPredicateList(head);
    free(predicates);
}