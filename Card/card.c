#include "card.h"
#include "../Utilities/inputUtils.h"
#define CARD_HOLDER_NAME_MAX 24
#define CARD_HOLDER_NAME_MIN 20

// Function to check the date is valid or not
static bool datevalid(int m, int y)
{
    // didn't specify how to test year
    if (m < 1 || m > 12)
    {
        return false;
    }
    return true;
}

EN_cardError_t getCardHolderName(ST_cardData_t *cardData)
{
    char inputName[80];
    fputs("Please Enter Card Holder Name\n", myOutStream);
    if (readStringFromUser(inputName, sizeof(inputName), CARD_HOLDER_NAME_MAX, CARD_HOLDER_NAME_MIN))
    {
        strcpy(cardData->cardHolderName, inputName);
        return CARD_OK;
    }
    else
    {
        return WRONG_NAME;
    }
}
EN_cardError_t getCardExpiryDate(ST_cardData_t *cardData)
{
#define CARD_EXPIRY_DATA_LENGTH 5
    char buffer[80];
    fputs("Please Enter Card Expiry Date \n", myOutStream);
    if (readStringFromUser(buffer, sizeof(buffer), CARD_EXPIRY_DATA_LENGTH, CARD_EXPIRY_DATA_LENGTH))
    {
        // The string is exactly 5 chars
        int month, year;
        if (buffer[2] != '/')
        {
            return WRONG_EXP_DATE;
        }
        // it has a \ at the 3rd position
        int itemsRead = sscanf(buffer, "%d/%d", &month, &year);
        if (itemsRead != 2)
        {
            return WRONG_EXP_DATE;
        }
        // it has 2 integers one before and one after the slash
        if (!datevalid(month, year))
        {
            return WRONG_EXP_DATE;
        }
        strcpy(cardData->cardExpirationDate,buffer);
        return CARD_OK;
    }
    else
    {
        return WRONG_EXP_DATE;
    }
}
EN_cardError_t getCardPAN(ST_cardData_t *cardData)
{
    #define CARD_PAN_MAX_LENGTH 19
    #define CARD_PAN_MIN_LENGTH 16
    char buffer[80];
    fputs("Please Enter Card Primary Account Number\n", myOutStream);
    if (readStringFromUser(buffer, sizeof(buffer), CARD_PAN_MAX_LENGTH, CARD_PAN_MIN_LENGTH))
    {
        if(isLuhnNumber(buffer)){
            strcpy(cardData->primaryAccountNumber,buffer);
            return CARD_OK;
        }
    }
    return WRONG_PAN;
}

void getCardHolderNameTest(void)
{
    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass --> 22 Chars", .expectedResult = CARD_OK, .inputData = "Hassan Saied Hassan Ma"},
        {.testCaseName = "Pass --> 20 Chars", .expectedResult = CARD_OK, .inputData = "Hassan Saied Hassan "},
        {.testCaseName = "Pass --> 24 Chars", .expectedResult = CARD_OK, .inputData = "Hassan Saied Hassan Mahm"},
        {.testCaseName = "Fail --> 0 Chars", .expectedResult = WRONG_NAME, .inputData = ""},
        {.testCaseName = "Fail --> 6 Chars", .expectedResult = WRONG_NAME, .inputData = "Hassan"},
        {.testCaseName = "Fail --> 34 Chars", .expectedResult = WRONG_NAME, .inputData = "Hassan Saied Hassan Mahmoud Hassan"},
    };
    ST_cardData_t objectUnderTest;
    runTestCases(testCases, sizeof(testCases) / sizeof(testCases[0]), "getCardHolderName", getCardHolderName, &objectUnderTest);
}
void getCardExpiryDateTest(void)
{
    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass --> Valid Date", .expectedResult = CARD_OK, .inputData = "06/25"},
        {.testCaseName = "Fail --> Short Date", .expectedResult = WRONG_EXP_DATE, .inputData = "6/25"},
        {.testCaseName = "Fail --> invalid month", .expectedResult = WRONG_EXP_DATE, .inputData = "15/25"},
        {.testCaseName = "Fail --> 0 Chars", .expectedResult = WRONG_EXP_DATE, .inputData = ""},
        {.testCaseName = "Fail --> Long Date", .expectedResult = WRONG_EXP_DATE, .inputData = "06/12/25"},
        {.testCaseName = "Fail --> Wrong format", .expectedResult = WRONG_EXP_DATE, .inputData = "06\\25"},
    };
    ST_cardData_t objectUnderTest;
    runTestCases(testCases, sizeof(testCases) / sizeof(testCases[0]), "getCardExpiryDate", getCardExpiryDate, &objectUnderTest);
}
void getCardPANTest (void)
{
        ST_testCase_t testCases[] = {
        {.testCaseName = "Pass --> Valid PAN 16", .expectedResult = CARD_OK, .inputData = "1077111724469458"},
        {.testCaseName = "Fail --> Invalid PAN 16", .expectedResult = WRONG_PAN, .inputData = "1077111724469459"},
        {.testCaseName = "Pass --> Valid PAN 19", .expectedResult = CARD_OK, .inputData = "8435417096038081209"},
        {.testCaseName = "Fail --> 0 Chars", .expectedResult = WRONG_PAN, .inputData = ""},
        {.testCaseName = "Fail --> Long PAN", .expectedResult = WRONG_PAN, .inputData = "10771117244694581077111724469459"},
        {.testCaseName = "Fail --> Short PAN", .expectedResult = WRONG_PAN, .inputData = "10771"},
    };
    ST_cardData_t objectUnderTest;
    runTestCases(testCases, sizeof(testCases) / sizeof(testCases[0]), "getCardPAN", getCardPAN, &objectUnderTest);
}