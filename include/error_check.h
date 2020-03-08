#ifndef ERROR_CHECK_H
#define ERROR_CHECK_H

#include <stdio.h>
#include <stdlib.h>

#define NULL_POINTER_ERROR_CHECK(pointer, file, func, line)                                 \
{                                                                                           \
    if (pointer == NULL){                                                                   \
        printf("Error at file: %s, function: %s, line: %d\n", file, func, line);            \
        perror("Error");                                                                    \
        exit(EXIT_FAILURE);                                                                 \
    }                                                                                       \
}


#define NEGATIVE_VALUE_ERROR_CHECK(pointer, file, func, line)                               \
{                                                                                           \
    if (pointer < 0){                                                                       \
        printf("Error at file: %s, function: %s, line: %d\n", file, func, line);            \
        perror("Error");                                                                    \
        exit(EXIT_FAILURE);                                                                 \
    }                                                                                       \
}

#endif