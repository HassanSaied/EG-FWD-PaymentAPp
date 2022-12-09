#include "server.h"


static ST_accountsDB_t* getAccountsDB(size_t * dbLength){
    static ST_accountsDB_t accountsDB[255] = {
    {.state = RUNNING, .balance = 2000, .primaryAccountNumber = "8989374615436851"},
    {.state = BLOCKED, .balance = 100000.0, .primaryAccountNumber = "5807007076043875"},
    {.state = BLOCKED, .balance = 12, .primaryAccountNumber = "4819700183242427"},
    {.state = RUNNING, .balance = 3750, .primaryAccountNumber = "7356286777482761"},
    {.state = RUNNING, .balance = 1230, .primaryAccountNumber = "4031147774727170"},
};
    FILE * fptr = fopen("Accounts  DB.txt","r");
    if(fptr == NULL){
        *dbLength = 0;
    }else{
        while (fread(&accountsDB[*dbLength],sizeof(ST_accountsDB_t),1,fptr) == 1)
        {
            *dbLength += 1;
        }
        fclose(fptr);
    }
    return accountsDB;
}
static ST_transaction_t * getTransactionDB(size_t * dbLength){
    static ST_transaction_t transactionDB[255] = {0};
    FILE * fptr = fopen("Transactions DB.txt","r");
    if(fptr == NULL){
        *dbLength = 0;
    }else{
        while (fread(&transactionDB[*dbLength],sizeof(ST_transaction_t),1,fptr) == 1)
        {
            *dbLength += 1;
        }
        fclose(fptr);
    }
    return transactionDB;
}
static bool saveTransactionData(ST_transaction_t * transData){
    FILE * fptr = fopen("Transactions DB.txt","a");
    int numWritten = fwrite(transData,sizeof(ST_transaction_t),1,fptr);
    fclose(fptr);
    return numWritten == 1;
}

EN_transState_t recieveTransactionData(ST_transaction_t * transData)
{
    char * cardPAN = transData->cardHolderData.primaryAccountNumber;
    size_t accountsDBLength;
    ST_accountsDB_t * accountsDB = getAccountsDB(&accountsDBLength);
    for (size_t i = 0; i < accountsDBLength; i++)
    {
        if(strcmp(cardPAN,accountsDB[i].primaryAccountNumber) == 0){
            //we found the account
            if(accountsDB[i].balance < transData->terminalData.transAmount){
                return DECLINED_INSUFFECIENT_FUND;
            }
            if(accountsDB[i].state == BLOCKED){
                return DECLINED_STOLEN_CARD;
            }
            if(!saveTransactionData(transData)){
                return INTERNAL_SERVER_ERROR ;
            }
            return APPROVED;
        }
    }
    return FRAUD_CARD;
}
EN_serverError_t isValidAccount(ST_cardData_t *cardData, ST_accountsDB_t *accountRefrence){
    char * cardPAN = cardData->primaryAccountNumber;
    size_t accountsDBLength;
    ST_accountsDB_t * accountsDB = getAccountsDB(&accountsDBLength);
    for (size_t i = 0; i < accountsDBLength; i++)
    {
        if(strcmp(cardPAN,accountsDB[i].primaryAccountNumber) == 0){
            //we found the account
            accountRefrence = &accountsDB[i];
            return SERVER_OK;
        }
    }
    accountRefrence = NULL;
    return ACCOUNT_NOT_FOUND;
}
EN_serverError_t isBlockedAccount(ST_accountsDB_t *accountRefrence){
   return accountRefrence->state == BLOCKED ?  BLOCKED_ACCOUNT : SERVER_OK;

}
EN_serverError_t isAmountAvailable(ST_terminalData_t *termData, ST_accountsDB_t *accountRefrence){
    return termData->transAmount > accountRefrence->balance ? LOW_BALANCE : SERVER_OK;

}
EN_serverError_t saveTransaction(ST_transaction_t *transData){
    static sequenceNumber = 0;
    transData->transactionSequenceNumber +=1;
    saveTransactionData(transData);
    listSavedTransactions();
    return SERVER_OK;
}
static void getStateString(EN_transState_t state, char * buffer){
    switch (state)
    {
    case APPROVED:
        strcpy(buffer,"Approved");
        break;
    case DECLINED_INSUFFECIENT_FUND:
        strcpy(buffer,"Insuffecient Funds");
        break;
    case  DECLINED_STOLEN_CARD:
        strcpy(buffer,"Stolen Card");
    break;
    case FRAUD_CARD:
        strcpy(buffer, "Fraud Card");
        break;
    case INTERNAL_SERVER_ERROR:
        strcpy(buffer,"Internal Server Error");    
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
        fprintf(myOutStream, "Transaction Sequence Number: %d", transactions[i].transactionSequenceNumber);
        fprintf(myOutStream, "Transaction Date: %s", transactions[i].terminalData.transactionDate);
        fprintf(myOutStream, "Transaction Amount: %d", transactions[i].terminalData.transAmount);
        getStateString(transactions[i].transState,buffer);
        fprintf(myOutStream, "Transaction State: %s", buffer);
        fprintf(myOutStream, "Terminal Max Amount: %d", transactions[i].terminalData.maxTransAmount);
        fprintf(myOutStream, "Cardholder Name: %s", transactions[i].cardHolderData.cardHolderName);
        fprintf(myOutStream, "PAN: %s", transactions[i].cardHolderData.primaryAccountNumber);
        fprintf(myOutStream, "Card Expiration Date: %s", transactions[i].cardHolderData.cardExpirationDate);
        fputs("#########################\n", myOutStream);
    }
}