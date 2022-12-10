#include "server.h"
static ST_accountsDB_t serverAccountsDB[255] = {
    {.state = RUNNING, .balance = 2000, .primaryAccountNumber = "8989374615436851"},
    {.state = BLOCKED, .balance = 100000.0, .primaryAccountNumber = "5807007076043875"},
    {.state = BLOCKED, .balance = 12, .primaryAccountNumber = "4819700183242427"},
    {.state = RUNNING, .balance = 3750, .primaryAccountNumber = "7356286777482761"},
    {.state = RUNNING, .balance = 1230, .primaryAccountNumber = "4031147774727170"},
};
static size_t serverNumSavedAccounts = 5;

static ST_accountsDB_t *getAccountsDB(size_t *dbLength)
{
    FILE *fptr = fopen("Accounts  DB.txt", "r");
    if (fptr != NULL)
    {
        serverNumSavedAccounts = 0;
        while (fread(&serverAccountsDB[serverNumSavedAccounts], sizeof(ST_accountsDB_t), 1, fptr) == 1)
        {
            serverNumSavedAccounts += 1;
        }
        fclose(fptr);
    }
    *dbLength = serverNumSavedAccounts;
    return serverAccountsDB;
}
static ST_transaction_t *getTransactionDB(size_t *dbLength)
{
    static ST_transaction_t transactionDB[255] = {0};
    FILE *fptr = fopen("Transactions DB.txt", "r");
    if (fptr == NULL)
    {
        *dbLength = 0;
    }
    else
    {
        *dbLength = 0;
        while (fread(&transactionDB[*dbLength], sizeof(ST_transaction_t), 1, fptr) == 1)
        {
            *dbLength += 1;
        }
        fclose(fptr);
    }
    return transactionDB;
}
static bool saveTransactionData(ST_transaction_t *transData)
{
    FILE *fptr = fopen("Transactions DB.txt", "a");
    int numWritten = fwrite(transData, sizeof(ST_transaction_t), 1, fptr);
    fclose(fptr);
    return numWritten == 1;
}
static bool uppdateAccountsDB(ST_accountsDB_t *accountsDB, size_t length)
{
    FILE *fptr = fopen("Accounts  DB.txt", "w");
    if (fptr == NULL)
    {
        return false;
    }
    bool retVal = fwrite(accountsDB, sizeof(ST_accountsDB_t), length, fptr) == length;
    fclose(fptr);
    return retVal;
}

EN_transState_t recieveTransactionData(ST_transaction_t *transData)
{
    transData->transState = APPROVED;

    ST_accountsDB_t *userAccount;
    if (isValidAccount(&transData->cardHolderData, &userAccount) == ACCOUNT_NOT_FOUND)
    {
        fprintf(myOutStream, "%s\n", "Declined Invalid Account");
        transData->transState = FRAUD_CARD;
    }
    if(transData->transState == APPROVED && (isBlockedAccount(userAccount) == BLOCKED_ACCOUNT)){
        fprintf(myOutStream, "%s\n", "Stolen Card");
        transData->transState = DECLINED_STOLEN_CARD;
    }
    if (transData->transState == APPROVED && (isAmountAvailable(&transData->terminalData, userAccount) == LOW_BALANCE))
    {
        fprintf(myOutStream, "%s\n", "Declined Insuffecient Funds");
        transData->transState = DECLINED_INSUFFECIENT_FUND;
    }
    if(transData ->transState == APPROVED){
        userAccount->balance -= transData->terminalData.transAmount;
        transData->transState = saveTransaction(transData);
        uppdateAccountsDB(serverAccountsDB, serverNumSavedAccounts);
    }else{
        saveTransaction(transData);

    }
    // update the accounts in the file because we adjusted the balance
    return transData->transState;
}
EN_serverError_t isValidAccount(ST_cardData_t *cardData, ST_accountsDB_t **accountRefrence)
{
    char *cardPAN = cardData->primaryAccountNumber;
    size_t accountsDBLength;
    ST_accountsDB_t *accountsDB = getAccountsDB(&accountsDBLength);
    for (size_t i = 0; i < accountsDBLength; i++)
    {
        if (strcmp(cardPAN, accountsDB[i].primaryAccountNumber) == 0)
        {
            // we found the account
            *accountRefrence = &accountsDB[i];
            return SERVER_OK;
        }
    }
    *accountRefrence = NULL;
    return ACCOUNT_NOT_FOUND;
}
EN_serverError_t isBlockedAccount(ST_accountsDB_t *accountRefrence)
{
    return accountRefrence->state == BLOCKED ? BLOCKED_ACCOUNT : SERVER_OK;
}
EN_serverError_t isAmountAvailable(ST_terminalData_t *termData, ST_accountsDB_t *accountRefrence)
{
    return termData->transAmount > accountRefrence->balance ? LOW_BALANCE : SERVER_OK;
}
EN_serverError_t saveTransaction(ST_transaction_t *transData)
{
    FILE *fptr = fopen("Transactions DB.txt", "r");
    int sequenceNumber = 0;
    if (fptr != NULL)
    {
        fseek(fptr, -1 * sizeof(ST_transaction_t), SEEK_END);
        ST_transaction_t lastTransaction;
        fread(&lastTransaction, sizeof(ST_transaction_t), 1, fptr);
        sequenceNumber = lastTransaction.transactionSequenceNumber + 1;
        fclose(fptr);
    }

    transData->transactionSequenceNumber = sequenceNumber;
    saveTransactionData(transData);
    listSavedTransactions();
    sequenceNumber += 1;
    return SERVER_OK;
}
static void getStateString(EN_transState_t state, char *buffer)
{
    switch (state)
    {
    case APPROVED:
        strcpy(buffer, "Approved");
        break;
    case DECLINED_INSUFFECIENT_FUND:
        strcpy(buffer, "Insuffecient Funds");
        break;
    case DECLINED_STOLEN_CARD:
        strcpy(buffer, "Stolen Card");
        break;
    case FRAUD_CARD:
        strcpy(buffer, "Fraud Card");
        break;
    case INTERNAL_SERVER_ERROR:
        strcpy(buffer, "Internal Server Error");
    default:
        break;
    }
}
void listSavedTransactions(void)
{
    size_t transactionDBLength;
    ST_transaction_t *transactions = getTransactionDB(&transactionDBLength);
    char buffer[80];
    for (size_t i = 0; i < transactionDBLength; i++)
    {
        fputs("#########################\n", myOutStream);
        fprintf(myOutStream, "Transaction Sequence Number: %d\n", transactions[i].transactionSequenceNumber);
        fprintf(myOutStream, "Transaction Date: %s\n", transactions[i].terminalData.transactionDate);
        fprintf(myOutStream, "Transaction Amount: %f\n", transactions[i].terminalData.transAmount);
        getStateString(transactions[i].transState, buffer);
        fprintf(myOutStream, "Transaction State: %s\n", buffer);
        fprintf(myOutStream, "Terminal Max Amount: %f\n", transactions[i].terminalData.maxTransAmount);
        fprintf(myOutStream, "Cardholder Name: %s\n", transactions[i].cardHolderData.cardHolderName);
        fprintf(myOutStream, "PAN: %s\n", transactions[i].cardHolderData.primaryAccountNumber);
        fprintf(myOutStream, "Card Expiration Date: %s\n", transactions[i].cardHolderData.cardExpirationDate);
        fputs("#########################\n", myOutStream);
    }
}

void isValidAccountTest(void)
{
    ST_cardData_t firstCardData = {
        .primaryAccountNumber = "8989374615436851",
    };
    ST_cardData_t secondCardData = {
        .primaryAccountNumber = "8393029623087902",
    };
    ST_accountsDB_t *accountsReference;
    void *firstTestCase[] = {&firstCardData, &accountsReference};
    void *secondTestCase[] = {&secondCardData, &accountsReference};

    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass, found account", .inputData = "8989374615436851", .inputParams = firstTestCase, .expectedResult = SERVER_OK},
        {.testCaseName = "Fail, Account Not Found", .inputData = "8393029623087902", .inputParams = secondTestCase, .expectedResult = ACCOUNT_NOT_FOUND}};

    runTestCasesNumParams(testCases, sizeof(testCases) / sizeof(ST_testCase_t), "isValidAccount", isValidAccount, 2);
}

void isBlockedAccountTest(void)
{
    ST_cardData_t firstCardData = {
        .primaryAccountNumber = "8989374615436851",
    };
    ST_cardData_t secondCardData = {
        .primaryAccountNumber = "5807007076043875",
    };
    ST_accountsDB_t *firstaccountsReference, *secondAccountReference;
    isValidAccount(&firstCardData, &firstaccountsReference);
    isValidAccount(&secondCardData, &secondAccountReference);

    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass, Account Active", .inputData = "8989374615436851", .inputParams = &firstaccountsReference, .expectedResult = SERVER_OK},
        {.testCaseName = "Fail, Account Blocked", .inputData = "5807007076043875", .inputParams = &secondAccountReference, .expectedResult = BLOCKED_ACCOUNT}};
    runTestCasesNumParams(testCases, sizeof(testCases) / sizeof(ST_testCase_t), "isBlockedAccount", isBlockedAccount, 1);
}

void isAmountAvailableTest(void)
{
    ST_cardData_t firstCardData = {
        .primaryAccountNumber = "8989374615436851",
    };
    ST_cardData_t secondCardData = {
        .primaryAccountNumber = "4031147774727170",
    };
    ST_accountsDB_t *firstaccountsReference, *secondAccountReference;
    isValidAccount(&firstCardData, &firstaccountsReference);
    isValidAccount(&secondCardData, &secondAccountReference);
    ST_terminalData_t terminalData = {.transAmount = 1500};
    void * firstCase[] = {&terminalData,firstaccountsReference};
    void * secondCase[] = {&terminalData,secondAccountReference};

    ST_testCase_t testCases[] = {
        {.testCaseName = "Pass, Amount Valid", .inputData = "2000 Balance, 1500 Trasfer", .inputParams = firstCase, .expectedResult = SERVER_OK},
        {.testCaseName = "Fail, Exceed Amount", .inputData = "1230 Balance, 1500 Transfer", .inputParams = secondCase, .expectedResult = LOW_BALANCE}};
    runTestCasesNumParams(testCases, sizeof(testCases) / sizeof(ST_testCase_t), "isAmountAvailable", isAmountAvailable, 2);
}

void saveTransactionTest(){
    remove("Transactions DB.txt");
    fputs("Tester Name: Hassan Saied\n",myOutStream);
    fprintf(myOutStream,"Function Name: %s\n","saveTransaction");
    ST_cardData_t cardData = {.cardHolderName = "Hassan Saied Hassan Ma",.cardExpirationDate = "10/26",.primaryAccountNumber = "8989374615436851"};
    ST_terminalData_t terminalData = {.maxTransAmount = 10000,.transactionDate = "12/10/2022",.transAmount = 500};
    ST_transaction_t transaction1 = {.cardHolderData = cardData,.terminalData = terminalData,.transState = APPROVED};
    ST_transaction_t transaction2 = {.cardHolderData = cardData,.terminalData = terminalData,.transState = DECLINED_INSUFFECIENT_FUND};
    fputs("Test Case 1\n",myOutStream);
    fputs("Expected Result \n",myOutStream);
    fputs("#########################\n\
Transaction Sequence Number: 0 \n\
Transaction Date: 12/10/2022 \n\
Transaction Amount: 500.000000 \n\
Transaction State: Approved \n\
Terminal Max Amount: 10000.000000 \n\
Cardholder Name: Hassan Saied Hassan Ma \n\
PAN: 8989374615436851 \n\
Card Expiration Date: 10/26 \n\
#########################\n",myOutStream);
    fputs("Actual Result \n",myOutStream);


    saveTransaction(&transaction1);
    fputs("Test Case 2\n",myOutStream);
    fputs("Expected Result \n",myOutStream);
    fputs("#########################\n\
Transaction Sequence Number: 0\n\
Transaction Date: 12/10/2022\n\
Transaction Amount: 500.000000\n\
Transaction State: Approved\n\
Terminal Max Amount: 10000.000000\n\
Cardholder Name: Hassan Saied Hassan Ma\n\
PAN: 8989374615436851\n\
Card Expiration Date: 10/26\n\
#########################\n\
#########################\n\
Transaction Sequence Number: 1\n\
Transaction Date: 12/10/2022\n\
Transaction Amount: 500.000000\n\
Transaction State: Insuffecient Funds\n\
Terminal Max Amount: 10000.000000\n\
Cardholder Name: Hassan Saied Hassan Ma\n\
PAN: 8989374615436851\n\
Card Expiration Date: 10/26\n\
#########################\n",myOutStream);
    fputs("Actual Result \n",myOutStream);
    saveTransaction(&transaction2);
}

void listSavedTransactionsTest(void)
{
    saveTransactionTest();
}