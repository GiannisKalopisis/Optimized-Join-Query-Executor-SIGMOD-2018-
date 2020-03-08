#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "error_check.h"
#include "general_functions.h"


int getSpaces(char *array) {

    int count = 0;
    char *p = array;
    while(*p != '\0') {
        if (*(p) == ' ') {
            count++;
        }
        p++ ;
    }
    return count;
}


int getAmpersand(char *array){
    int count = 0;
    char *p = array;
    while(*p != '\0') {
        if (*(p) == '&') {
            count++;
        }
        p++ ;
    }
    return count;
}


int getDots(char *array){
    int count = 0;
    char *p = array;
    while(*p != '\0') {
        if (*(p) == '.') {
            count++;
        }
        p++ ;
    }
    return count;
}


void getFileSize(char *fileName, int *numRows, int *numColumns) {

    int sizeCol = 0, sizeRow = 0;

    FILE *file;
    file = fopen(fileName, "r");
    NULL_POINTER_ERROR_CHECK(file, __FILE__, __func__, __LINE__);

    char *line = NULL;
    size_t len = 0;

    if (getline(&line, &len, file) != -1) {
        char * token = strtok(line, " \t\n");
        while (token != NULL){
            sizeCol++;
            token = strtok(NULL, " \t\n");
        }
        sizeRow++;
    }

    while (getline(&line, &len, file) != -1) {
        sizeRow++;
    }

    fclose(file);
    free(line);

    (*numColumns) = sizeCol;
    (*numRows) = sizeRow;
}


void arrayInit(uint64_t ***array, int columns, int rows){

    (*array) = malloc(rows * sizeof(uint64_t *));
    NULL_POINTER_ERROR_CHECK(*array, __FILE__, __func__, __LINE__);

    int i = 0;
    for (i = 0; i < rows; i ++){
        (*array)[i] = malloc(columns * sizeof(uint64_t));
        NULL_POINTER_ERROR_CHECK((*array)[i], __FILE__, __func__, __LINE__);
    }

}


void readArray(uint64_t **array, int columns, int rows, char *fileName){

    FILE *file = fopen(fileName, "r");
    NULL_POINTER_ERROR_CHECK(file, __FILE__, __func__, __LINE__);

    char *line = NULL, *token;
    size_t len = 0;
    int i = 0, j;

    while (getline(&line, &len, file) != -1) {
        j = 0;
        token = strtok(line, " \t\n");
        while (token != NULL){
            array[i][j] = (int32_t) atoi(token);
            j++;
            token = strtok(NULL, " \t\n");
        }
        i++;
    }

    free(line);
    fclose(file);

}


void arrayDestroy(uint64_t **array, int columns){

    int i;
    for (i = 0; i < columns; ++i) {
        free(array[i]);
    }
    free(array);

}


int getDigits(uint64_t number){

    uint64_t temp = number;
    int counter = 0;

    while (temp != 0){
        counter++;
        //fprintf(stderr, " counter:%d temp:%lu", counter, temp);
        temp = temp/10;
    }
    //counter++;
    //printf("%d", counter);
    return counter;
}

