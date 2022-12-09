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
    sscanf(termData->transactionDate, "*%d/%d/%d",&terminalDay, &terminalMonth, &terminalYear);
    sscanf(cardData->cardExpirationDate, "%d/%d", &cardMonth, &cardYear);
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
    if (readStringFromUser(buffer, sizeof(buffer), sizeof(buffer) - 1, 0))
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