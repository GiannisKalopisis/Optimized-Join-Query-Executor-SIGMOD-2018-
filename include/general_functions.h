#ifndef GENERAL_FUNCTIONS_H
#define GENERAL_FUNCTIONS_H

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

int getSpaces(char *);

int getAmpersand(char *);

int getDots(char *);

void getFileSize(char *, int *, int *);

void arrayInit(uint64_t ***, int, int);

void readArray(uint64_t **, int, int, char *);

void arrayDestroy(uint64_t **, int);

int getDigits(uint64_t);

#endif