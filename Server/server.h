#ifndef SERVER_H
#define SERVER_H
#include "../Card/card.h"
#include "../Terminal/terminal.h"


typedef enum EN_transState_t
{
     APPROVED, DECLINED_INSUFFECIENT_FUND, DECLINED_STOLEN_CARD, FRAUD_CARD, INTERNAL_SERVER_ERROR
}EN_transState_t;

typedef struct ST_transaction_t
{
     ST_cardData_t cardHolderData;
     ST_terminalData_t terminalData;
     EN_transState_t transState;
     uint32_t transactionSequenceNumber;
}ST_transaction_t;

typedef enum EN_serverError_t 
{
     SERVER_OK, SAVING_FAILED, TRANSACTION_NOT_FOUND, ACCOUNT_NOT_FOUND, LOW_BALANCE, BLOCKED_ACCOUNT
}EN_serverError_t ;


typedef enum EN_accountState_t
{
     RUNNING,
     BLOCKED
}EN_accountState_t;

typedef struct ST_accountsDB_t
{
     float balance;
     EN_accountState_t state;
     uint8_t primaryAccountNumber[20];
}ST_accountsDB_t;

EN_transState_t recieveTransactionData(ST_transaction_t *transData);
/**
 * Although the requirement was that the function isValidAccount would take two pointers as
 * an argument (ST_cardData_t *cardData, ST_accountsDB_t *accountRefrence), I decided for the second
 * parameter to be a pointer to pointer, because this function should give me a reference to that account
 * in the database, not just copy the account data, to whatever location I point it to.
 * That was enforced by the requirement If the PAN doesn't exist will return ACCOUNT_NOT_FOUND and the account reference will be NULL,
 *  else will return SERVER_OK and return a reference to this account in the DB. 
 * When I declare the function taking apointer to the accounts reference there is no way to change whatever this pointer points to
 * it's an address that is copied into the function, I can only change what this address points to, not the address it self
 * so for accountReference = NULL to have any effect once I am returned form this funciton, it needs to be passed like this
 * ST_accountsDB_t **accountRefrence
*/
EN_serverError_t isValidAccount(ST_cardData_t *cardData, ST_accountsDB_t **accountRefrence);
EN_serverError_t isBlockedAccount(ST_accountsDB_t *accountRefrence);
EN_serverError_t isAmountAvailable(ST_terminalData_t *termData, ST_accountsDB_t *accountRefrence);
EN_serverError_t saveTransaction(ST_transaction_t *transData);
void listSavedTransactions(void);
void recieveTransactionDataTest(void);
void isValidAccountTest(void);
void isBlockedAccountTest(void);
void isAmountAvailableTest(void);
void saveTransactionTest(void);
void listSavedTransactionsTest(void);
#endif //SERVER_H