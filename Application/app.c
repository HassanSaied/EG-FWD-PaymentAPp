#include <stdio.h>
#include "app.h"
#include "../Utilities/inputUtils.h"


int main(int argc, char ** argv){
    initStreams();
    remove("Accounts  DB.txt");
    remove("Transactions DB.txt");
    
    // getCardHolderNameTest();
    // getCardExpiryDateTest();
    // getCardPANTest();

    // isCardExpriedTest();
    // getTransactionDateTest();
    // getTransactionAmountTest();
    // isBelowMaxAmountTest();
    // setMaxAmountTest();
    // isValidCardPANTest();

    isValidAccountTest();
    recieveTransactionDataTest();
    isBlockedAccountTest();
    isAmountAvailableTest();
    saveTransactionTest();
    listSavedTransactionsTest();

    // appStartTest();

    return 0;
}
void appStartTest(void)
{
    remove("Accounts  DB.txt");
    remove("Transactions DB.txt");
    myOutStream = fopen("AppTestOutput.txt","w");
    char testCaseInputs[5][5][80] = {
            {"Hassan Saied Hassan Ma","10/26","8989374615436851","10000","1200"},
            {"Hassan Saied Hassan Ma","10/26","8989374615436851","10000","12000"},
            {"Hassan Saied Hassan Ma","10/26","8989374615436851","10000","1000"},
            {"Hassan Saied Hassan Ma","10/20","8989374615436851","10000","1200"},
            {"Hassan Saied Hassan Ma","10/26","5807007076043875","100000","12000"},
        };
    for (size_t i = 0; i < 5; i++)
    {
        FILE * fptr = fopen("AppTestCase.txt","w");
        for (size_t j = 0; j < 5; j++)
        {
            fprintf(fptr,"%s\n",testCaseInputs[i][j]);
        }
        fclose(fptr);
        myInStream = fopen("AppTestCase.txt","r");
        fprintf(myOutStream,"Test Case %ld\n",i);
        appStart();
        fclose(myInStream);
        fputs("==================================\n",myOutStream);
        myInStream = stdin;
    }
    fclose(myOutStream);

    
}

void appStart(void){
    ST_cardData_t cardData;
    ST_terminalData_t terminalData;
    ST_transaction_t transactionData;

    //Card Data
    getCardHolderName(&cardData);
    getCardExpiryDate(&cardData);
    getCardPAN(&cardData);
    transactionData.cardHolderData = cardData;

    //Terminal Data
    //get the max amount as an input for test cases later on
    EN_terminalError_t setMaxAmountErr = INVALID_MAX_AMOUNT;
    do{
        float userMaxAmount;
        fprintf(myOutStream,"Enter the Terminal max amount\n");
        if(fscanf(myInStream,"%f",&userMaxAmount) == 1){
            setMaxAmountErr =  setMaxAmount(&terminalData,userMaxAmount);
        }
        //remove the \n from the stream
        fgetc(myInStream);
    }while (setMaxAmountErr != TERMINAL_OK);
    // setMaxAmount(&terminalData,10000);

    getTransactionDate(&terminalData);
    if(isCardExpired(&cardData,&terminalData) == EXPIRED_CARD){
        fprintf(myOutStream,"%s\n","Declined Expired Card");
        return;
    }
    getTransactionAmount(&terminalData);
    if(isBelowMaxAmount(&terminalData) == EXCEED_MAX_AMOUNT){
        fprintf(myOutStream,"%s\n","Declined Amount Exceeding Limit");
        return;
    }
    if(isValidCardPAN(&cardData) == INVALID_CARD){
        fprintf(myOutStream,"%s\n","Declined Invalid Card");
        return;
    }
    transactionData.terminalData = terminalData;

    //Transaction Data
    recieveTransactionData(&transactionData);
}