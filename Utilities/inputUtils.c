#include "inputUtils.h"

static int testCaseIndex = 0;

FILE * myInStream = NULL;
FILE * myOutStream = NULL;

void initStreams(){
    myInStream = stdin;
    myOutStream = stdout;
}

bool readStringFromUser(char * buffer,unsigned int buffer_size, unsigned int size_max, unsigned int size_min)
{
    char * readStr = fgets(buffer,buffer_size,myInStream);
    if(readStr != NULL){
        buffer[strcspn(buffer, "\r\n")] = 0;
        //remove the new line that gets appended in the end
        unsigned int numInputChars = strlen(readStr);
        if(numInputChars > size_max || numInputChars < size_min){
            return false;
        }else{
            return true;
        }
    }
    return false;

}

void runTestCases(ST_testCase_t * testCases, int numTestCases,const char * functionName,testableFunction_t functionUnderTest, void * params)
{
    for (size_t i = 0; i < numTestCases; i++)
    {
        testCases[i].inputParams = &params;
    }
    runTestCasesNumParams(testCases, numTestCases,functionName,functionUnderTest,1);

    // char inputFileName[80];
    // char ouputFileName[80];
    // sprintf(inputFileName,"TestCase%dInput.txt",testCaseIndex);
    // sprintf(ouputFileName,"TestCase%dOutput.txt",testCaseIndex);

    // FILE * fptr = fopen(inputFileName,"w");
    // for (size_t i = 0; i < numTestCases; i++)
    // {
    //     fprintf(fptr,"%s\n",testCases[i].inputData);
    // }
    // fclose(fptr);
    // //convert the input stream from stdin to the file with the test cases
    // myInStream = fopen(inputFileName,"r");
    // fputs("Tester Name: Hassan Saied\n",myOutStream);
    // fprintf(myOutStream,"Function Name: %s\n",functionName);
    // FILE* outputFilePtr = fopen(ouputFileName,"w");
    // int numPassTestCases = 0;
    // for (size_t i = 0; i < numTestCases; i++)
    // {
    //     fprintf(myOutStream,"Test Case %ld: %s\n",i+1,testCases[i].testCaseName);
    //     fprintf(myOutStream,"Input Data: %s\n",testCases[i].inputData);
    //     fprintf(myOutStream,"Expected Result: %d\n",testCases[i].expectedResult);
    //     myOutStream = outputFilePtr;
    //     int actualResult = functionUnderTest(params);
    //     fflush(myOutStream);
    //     myOutStream = stdout;
    //     fprintf(myOutStream,"Actual Result: %d\n",actualResult);
    //     if(testCases[i].expectedResult == actualResult){
    //         fprintf(myOutStream,"TEST OK\n");
    //         numPassTestCases += 1;
    //     }else{
    //         fprintf(myOutStream,"TEST FAIL\n");
    //     }
    // }
    // fclose(outputFilePtr);
    // fclose(myInStream);
    // myInStream = stdin;
    // fprintf(myOutStream,"Pass %d/%d\n",numPassTestCases,numTestCases);
    // fputs("====================================================\n",myOutStream);
    // testCaseIndex += 1;
}
void runTestCasesNumParams(ST_testCase_t * testCases, int numTestCases,const char * functionName,void * functionUnderTest, int numParams)
{
    char inputFileName[80];
    char ouputFileName[80];
    sprintf(inputFileName,"TestCase%dInput.txt",testCaseIndex);
    sprintf(ouputFileName,"TestCase%dOutput.txt",testCaseIndex);

    FILE * fptr = fopen(inputFileName,"w");
    for (size_t i = 0; i < numTestCases; i++)
    {
        fprintf(fptr,"%s\n",testCases[i].inputData);
    }
    fclose(fptr);
    //convert the input stream from stdin to the file with the test cases
    myInStream = fopen(inputFileName,"r");
    fputs("Tester Name: Hassan Saied\n",myOutStream);
    fprintf(myOutStream,"Function Name: %s\n",functionName);
    FILE* outputFilePtr = fopen(ouputFileName,"w");
    int numPassTestCases = 0;
    for (size_t i = 0; i < numTestCases; i++)
    {
        fprintf(myOutStream,"Test Case %ld: %s\n",i+1,testCases[i].testCaseName);
        fprintf(myOutStream,"Input Data: %s\n",testCases[i].inputData);
        fprintf(myOutStream,"Expected Result: %d\n",testCases[i].expectedResult);
        myOutStream = outputFilePtr;
        int actualResult;
        switch (numParams)
        {   
        case 0:{
            testableFunctionNoParams_t function = functionUnderTest;
            function();
        }
            break;
        case 1:{
            testableFunction_t function = functionUnderTest;
            actualResult = function(testCases[i].inputParams[0]);
        }
            break;
        case 2:{
            if(testCases[i].secondParamFloat){
                testableFunctionTwoParamsFloat_t function = functionUnderTest;
                actualResult = function(testCases[i].inputParams[0],*((float*)testCases[i].inputParams[1]));

            }else{
                testableFunctionTwoParams_t function = functionUnderTest;
                actualResult = function(testCases[i].inputParams[0],testCases[i].inputParams[1]);
            }
        }
            break;
        default:
            break;
        }
        fflush(myOutStream);
        myOutStream = stdout;
        fprintf(myOutStream,"Actual Result: %d\n",actualResult);
        if(testCases[i].expectedResult == actualResult){
            fprintf(myOutStream,"TEST OK\n");
            numPassTestCases += 1;
        }else{
            fprintf(myOutStream,"TEST FAIL\n");
        }
    }
    fclose(outputFilePtr);
    fclose(myInStream);
    myInStream = stdin;
    fprintf(myOutStream,"Pass %d/%d\n",numPassTestCases,numTestCases);
    fputs("====================================================\n",myOutStream);
    testCaseIndex += 1;
}

bool isLuhnNumber(char * number){
    int length = strlen(number);

    int sum = 0;
    bool first = true;
    for (int i = length - 2; i >= 0; i--)
    {
        if(first){
            int result = (number[i] - '0') * 2;
            sum += result / 10;
            sum += result % 10;
        }else{
            sum += number[i] - '0';
        }
        first = !first;
    }
    int c = (10 - (sum % 10)) % 10;
    return c == number[length-1] - '0';
}

