//
// Created by kalo-pc on 18/11/2018.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
//#include <time.h>

#include "../include/error_check.h"
#include "../include/relations.h"
#include "../include/SQL_QueryParser.h"
#include "../include/job_scheduler.h"
#include "../include/startup.h"

#define HARNESS 1

int main(int argc, char **argv) {

    init();

    ArrayHead *headRelationList;
    arrayHeadInit(&headRelationList);

#ifdef HARNESS
    char line[256];
    memset(line, '\0', 256 * sizeof(char));
#else
    char *line = NULL;
    size_t len = 0;
#endif

    ssize_t nread = 0;

#ifdef HARNESS
    char buffer[1];
    int counter = 0;

    while ((nread = read(0, buffer, sizeof(buffer))) > 0){  //0 is stdin

        if (buffer[0] != '\n') {
            line[counter] = buffer[0];
            counter++;
        }
        else {
            //fprintf(stderr, "Line 1: %s, size: %lu\n", line, nread);
            counter = 0;
            if (strcmp(line, "Done") == 0){
                break;
            }
            arrayListInsert(headRelationList, line);
            memset(line, '\0', 256 * sizeof(char));
        }
    }
#else
    while ((nread = getline(&line, &len, stdin)) != -1) {

        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
            --nread;
        }

        if (strcmp(line, "Done") == 0) {
            break;
        }
        //printf("Line 1: %s, size: %lu\n", line, nread);
        arrayListInsert(headRelationList, line);
    }
#endif


    /*
     *1 second delay
     */

#ifdef HARNESS
    char bigLine[2048];
    counter = 0;

    while ((nread = read(0, buffer, sizeof(buffer))) > 0){  //0 is stdin

        if (buffer[0] != '\n') {
            bigLine[counter] = buffer[0];
            counter++;
        }
        else {
            counter = 0;
            if (strcmp(bigLine, "F") == 0){
                continue;
            }
            //fprintf(stderr, "Line 2: %s, size: %lu\n", bigLine, nread);
            mainQueryParser(bigLine, headRelationList);
            memset(bigLine, '\0', 2048 * sizeof(char));
        }
    }
#else
    //printf("\n");

    FILE *f = fdopen(3, "r");
    NULL_POINTER_ERROR_CHECK(f, __FILE__, __func__, __LINE__);

    //struct timespec start, finish;
    //double timeTaken;

    //clock_gettime(CLOCK_MONOTONIC, &start);

    while ((nread = getline(&line, &len, f)) != -1) {

        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
            --nread;
        }

        if (strcmp(line, "F") == 0){
            continue;
        }
        //printf("Line 2: %s, size: %lu\n", line, nread);
        mainQueryParser(line, headRelationList);
    }

    // Temporary time count measure, before we connect our program with harness
    // clock_gettime(CLOCK_MONOTONIC, &finish);
    // timeTaken = finish.tv_sec - start.tv_sec;
    // timeTaken += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    // printf("\nMain query parser took %f seconds to execute \n", timeTaken);


    fclose(f);
    free(line);
#endif

    arrayListDestroy(headRelationList);

    destroy();

    return 0;
}
