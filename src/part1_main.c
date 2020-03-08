#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "RHJ.h"
#include "general_functions.h"
#include "relations.h"
#include "result.h"

//#define DEBUG_MAIN


int main(int argc, char **argv) {

    char *fileR = "./R_file.txt";
    char *fileS = "./S_file.txt";

    uint64_t **arrayR, **arrayS;

    int numRowsR, numRowsS, numColumnsR, numColumnsS;
    getFileSize(fileR, &numRowsR, &numColumnsR);
    getFileSize(fileS, &numRowsS, &numColumnsS);

    arrayInit(&arrayR, numColumnsR, numRowsR);
    arrayInit(&arrayS, numColumnsS, numRowsS);

    readArray(arrayR, numColumnsR, numRowsR, fileR);
    readArray(arrayS, numColumnsS, numRowsS, fileS);


    Relation *relationR, *relationS;

    rInit(&relationR, numColumnsR);
    rInit(&relationS, numColumnsS);

    int joinColumnR = 2;   // Columns to be joined (2nd column here)
    int joinColumnS = 2;

    if (joinColumnR > numRowsR) {
        printf("JoinColumnR too big! Choosing last column as default.\n");
        joinColumnR = numRowsR;
    }
    if (joinColumnS > numRowsS) {
        printf("JoinColumnS too big! Choosing last column as default.\n");
        joinColumnS = numRowsS;
    }


#ifdef DEBUG_MAIN
    printf("\nSize of R: [%d,%d], column to be joined: %d\n", numRowsR, numColumnsR, joinColumnR);
    printf("\nSize of S: [%d,%d] column to be joined: %d\n", numRowsS, numColumnsS, joinColumnS);
#endif

    rInsert(relationR, arrayR, joinColumnR - 1, numColumnsR);
    rInsert(relationS, arrayS, joinColumnS - 1, numColumnsS);

    ResultHead *res = RadixHashJoin(relationR, relationS);
    //resultPrintAll(res);

    //// Memory Deallocation ////

    resultDestroy(res);

    rDestroy(relationR);
    rDestroy(relationS);

    arrayDestroy(arrayR, numRowsR);
    arrayDestroy(arrayS, numRowsS);

    return 0;
}