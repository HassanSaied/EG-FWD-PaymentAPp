#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef int (*testableFunction_t)(void *);

typedef struct  ST_testCase_t
{
    char inputData[80];
    char testCaseName[255];
    int expectedResult;
} ST_testCase_t;

extern FILE * myOutStream;
extern FILE * myInStream;

void initStreams();
bool readStringFromUser(char * buffer,unsigned int buffer_size, unsigned int size_max, unsigned int size_min);
void runTestCases(ST_testCase_t * testCases, int numTestCases,const char * functionName,testableFunction_t functionUnderTest, void * params);
bool isLuhnNumber(char * number);

#endif