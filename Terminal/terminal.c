#include "terminal.h"

EN_terminalError_t getTransactionDate(ST_terminalData_t *termData)
{
    time_t current_time;
    time(&current_time);
    struct tm *localTimeValue = localtime(&current_time);
    sprintf(termData->transactionDate, "%02d/%02d/%04d", localTimeValue->tm_mday, localTimeValue->tm_mon + 1, localTimeValue->tm_year + 1900);
    return TERMINAL_OK;
}
EN_terminalError_t isCardExpired(ST_cardData_t *cardData, ST_terminalData_t *termData)
{
    int terminalDay,terminalMonth, terminalYear, cardMonth, cardYear;
    sscanf(termData->transactionDate, "%d/%d/%d",&terminalDay, &terminalMonth, &terminalYear);
    sscanf(cardData->cardExpirationDate, "%d/%d", &cardMonth, &cardYear);
    cardYear += 2000;
    if (terminalYear > cardYear || ((terminalYear == cardYear) && (terminalMonth > cardMonth)))
    {
        return EXPIRED_CARD;
    }
    else
    {
        return TERMINAL_OK;
    }
}
EN_terminalError_t getTransactionAmount(ST_terminalData_t *termData)
{
    char buffer[80];
    fprintf(myOutStream,"Please Enter transaction amount\n");
    if (readStringFromUser(buffer, sizeof(buffer), sizeof(buffer) - 1, 1))
    {
        float amount;
        sscanf(buffer, "%f", &amount);
        if (amount > 0)
        {
            termData->transAmount = amount;
            return TERMINAL_OK;
        }
    }
    return INVALID_AMOUNT;
}
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t *termData)
{
    if (termData->transAmount > termData->maxTransAmount)
    {
        return EXCEED_MAX_AMOUNT;
    }
    else
    {
        return TERMINAL_OK;
    }
}
EN_terminalError_t setMaxAmount(ST_terminalData_t *termData, float maxAmount)
{
    if (maxAmount > 0)
    {
        termData->maxTransAmount = maxAmount;
        return TERMINAL_OK;
    }
    else
    {
        return INVALID_MAX_AMOUNT;
    }
}
EN_terminalError_t isValidCardPAN(ST_cardData_t *cardData)
{
    if(isLuhnNumber(cardData->primaryAccountNumber)){
        return TERMINAL_OK;
    }else{
        return INVALID_CARD;
    }
}
void getTransactionDateTest(void){
    //There is no other case scenario, and handling the kernel giving me wrong time is really outside the scope of this project
    ST_terminalData_t terminalData;
    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass, Automatic Test Case",.inputData = "Getting Today's date",.expectedResult = TERMINAL_OK},
    };
    runTestCases(testCases,1,"getTransactionDate",getTransactionDate,&terminalData);
}

void isCardExpriedTest(void){
    ST_terminalData_t terminalData;
    getTransactionDate(&terminalData);
    ST_cardData_t firstCaseCard = {.cardExpirationDate = "12/26"};
    ST_cardData_t secondCaseCard = {.cardExpirationDate = "1/26"};
    ST_cardData_t thirdCaseCard = {.cardExpirationDate = "12/20"};
    ST_cardData_t fourthCaseCard = {.cardExpirationDate = "1/20"};

    void * firstCaseParams[] = {&firstCaseCard,&terminalData};
    void * secondCaseParams[] = {&secondCaseCard,&terminalData};
    void * thirdCaseParams[] = {&thirdCaseCard,&terminalData};
    void * fourthCaseParams[] = {&fourthCaseCard,&terminalData};
    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass, higher year",.inputData = "12/26",.inputParams = firstCaseParams,.expectedResult = TERMINAL_OK},
        {.testCaseName = "Pass, lower month, higher year",.inputData = "1/26",.inputParams = secondCaseParams,.expectedResult = TERMINAL_OK},
        {.testCaseName = "Fail, lower year",.inputData = "12/20",.inputParams = thirdCaseParams,.expectedResult = EXPIRED_CARD},
        {.testCaseName = "Fail, lower year and month",.inputData = "1/20",.inputParams = fourthCaseParams,.expectedResult = EXPIRED_CARD},
    };
    runTestCasesNumParams(testCases,4,"isCardExpired",isCardExpired,2);

}

void getTransactionAmountTest(void){
    //There is no other case scenario, and handling the kernel giving me wrong time is really outside the scope of this project
    ST_terminalData_t terminalData;
    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass, Amount > 0",.inputData = "10000",.expectedResult = TERMINAL_OK},
        {.testCaseName = "Fail, Amount < 0",.inputData = "-10000",.expectedResult = INVALID_AMOUNT},
    };
    runTestCases(testCases,2,"getTransactionAmount",getTransactionAmount,&terminalData);
}
void isBelowMaxAmountTest(void){

    ST_terminalData_t firstCaseData = {.maxTransAmount = 2000,.transAmount = 1000};
    ST_terminalData_t secondCaseData = {.maxTransAmount = 1000,.transAmount = 2000};
    void * firstParams[] = {&firstCaseData};
    void * secondParams[] = {&secondCaseData};

    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass", .inputData = "Max Amount 2000, trans Amount 1000",.inputParams = firstParams,.expectedResult = TERMINAL_OK},
        {.testCaseName = "Fail", .inputData = "Max Amount 1000, trans Amount 2000",.inputParams = secondParams,.expectedResult = EXCEED_MAX_AMOUNT}
    };
    runTestCasesNumParams(testCases,2,"isBelowMaxAmount",isBelowMaxAmount,1);

}
void setMaxAmountTest(void){
    ST_terminalData_t terminalData;
    int maxAmount1 = 1000, maxAmount2 = -1000;
    void * firstParams[] = {&terminalData,&maxAmount1};
    void * secondParams[] = {&terminalData,&maxAmount2};

    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass", .inputData = "1000",.inputParams = firstParams,.expectedResult = TERMINAL_OK,.secondParamFloat = true},
        {.testCaseName = "Fail", .inputData = "-1000",.inputParams = secondParams,.expectedResult = INVALID_MAX_AMOUNT ,.secondParamFloat = true}
    };
    runTestCasesNumParams(testCases,2,"setMaxAmount",setMaxAmount,2);
}

void isValidCardPANTest(void){
    ST_cardData_t firstCaseData = {.primaryAccountNumber = "8989374615436851"};
    ST_cardData_t secondCaseData = {.primaryAccountNumber = "8989374615436852"};
    void * firstParams[] = {&firstCaseData};
    void * secondParams[] = {&secondCaseData};

    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass", .inputData = "8989374615436851",.inputParams = firstParams,.expectedResult = TERMINAL_OK},
        {.testCaseName = "Fail", .inputData = "8989374615436852",.inputParams = secondParams,.expectedResult = INVALID_CARD}
    };
    runTestCasesNumParams(testCases,2,"isValidCardPAN",isValidCardPAN,1);

}