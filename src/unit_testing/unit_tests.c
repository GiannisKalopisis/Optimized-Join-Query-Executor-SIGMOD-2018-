#include "../../include/unit_general_functions.h"
#include "../../include/unit_relations.h"
#include "../../include/unit_result.h"
#include "../../include/unit_rhj.h"
#include "../../include/unit_SQL_QueryParser.h"
#include "../../include/unit_predicateParser.h"
#include "../../include/startup.h"
#include "../../include/unit_joinEnumeration.h"
#include "CUnit/Basic.h"

int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

int main()
{
    init();

    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    //// GENERAL FUNCTIONS SUITE ////

    /* add a suite to the registry */
    pSuite = CU_add_suite("General Functions Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ( (NULL == CU_add_test(pSuite, "test of GetFileSize", testGetFileSize)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of arrayInit", testArrayInit)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of readArray", testReadArray)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of getSpaces", testGetSpaces)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of getAmpersand", testGetAmpersand)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of getDots", testGetDots)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of getDigits", testGetDigits)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    //// RELATIONS SUITE ////

    /* add a suite to the registry */
    pSuite = CU_add_suite("Relations Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ( (NULL == CU_add_test(pSuite, "test of rInit", testrInit)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of rNewInsert", testrNewInsert)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of rInsert", testrInsert)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    if ( (NULL == CU_add_test(pSuite, "test of arrayHeadInit", testArrayHeadInit)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of arrayListInit", testArrayListInit)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    if ( (NULL == CU_add_test(pSuite, "test of arrayListInsert", testArrayListInsert)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of arrayListIndex", testArrayListIndex)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }




    //// RESULT SUITE ////

    /* add a suite to the registry */
    pSuite = CU_add_suite("Result Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ( (NULL == CU_add_test(pSuite, "test of resultInit", testResultInit)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of resultCreateNewNode", testResultCreateNewNode)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of resultIsLastNodeFull", testResultIsLastNodeFull)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of resultAppend", testResultAppend)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of resultGetTotalTuples", testResultGetTotalTuples)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of resultRestart", testResultRestart)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of intermediateResultInit", testIntermediateResultInit)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of intermediateResultCreateNewNode", testIntermediateResultCreateNewNode)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of intermediateResultAddJoinedRel", testIntermediateResultAddJoinedRel)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of intermediateResultIsContained", testIntermediateResultIsContained)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of intermediateResultGetNode", testIntermediateResultGetNode)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of intermediateResultUpdateSize", testIntermediateResultUpdateSize)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }



    //// RADIX HASH JOIN SUITE ////

    /* add a suite to the registry */


    pSuite = CU_add_suite("Radix Hash Join Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */

    if ( (NULL == CU_add_test(pSuite, "test of RHJ_stage1", testRHJ_stage1)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (NULL == CU_add_test(pSuite, "test of Index Functions (chainCreation, initializeBuckets, initializationIndex)",
                            testIndexFunctions)) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of RHJ_stage2", testRHJ_stage2)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of RHJ_stage3", testRHJ_stage3)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of RadixHashJoin", testRadixHashJoin)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    //// PREDICATE PARSER SUITE ////

    /* add a suite to the registry */
    pSuite = CU_add_suite("Predicate Parser Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    /* add the tests to the suite */
    if ( (NULL == CU_add_test(pSuite, "test of predicateListHeadInit", testPredicateListHeadInit)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of predicateListNodeCreation", testPredicateListNodeCreation)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of predicateListInsert", testPredicateListInsert)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of predicateListCreation", testPredicateListCreation)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    /*
    if ( (NULL == CU_add_test(pSuite, "test of updatePriority", testUpdatePriority)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    */
    if ( (NULL == CU_add_test(pSuite, "test of getHighestPriorityPredicate", testGetHighestPriorityPredicate)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }



    //// SQL QUERY PARSER SUITE ////

    /* add a suite to the registry */
    pSuite = CU_add_suite("SQL Query Parser Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    /* add the tests to the suite */
    if ( (NULL == CU_add_test(pSuite, "test of mainQueryParser", testMainQueryParser)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of SQL_arrayIndexing", testSQL_arrayIndexing)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of SQL_runFilterPredicate", testSQL_runFilterPredicate)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of SQL_runConjuctionPredicate", testSQL_runConjuctionPredicate)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }



    //// JOIN ENUMERATION SUITE ////

    /* add a suite to the registry */
    pSuite = CU_add_suite("Join Enumeration Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    /* add the tests to the suite */
    if ( (NULL == CU_add_test(pSuite, "test of existsInS", testExistsInS)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of MergeArray", testMergeArray)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of binomialCoeff", testBinomialCoeff)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of hashFunctionPosition", testHashFunctionPosition)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of structCompinationInit", testStructCompinationInit)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test of subsetsCompinations", testSubsetsCompinations)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }




    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    destroy();

    return CU_get_error();
}
