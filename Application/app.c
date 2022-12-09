#include <stdio.h>
#include "app.h"
#include "../Utilities/inputUtils.h"


int main(int argc, char ** argv){
    initStreams();
    fputs("Hello Wolrd!\n",myOutStream);
    // ST_cardData_t cardData;
    // fprintf(myOutStream,"CardNameResult %d\n",getCardHolderName(&cardData));
    // fputs(cardData.cardHolderName,myOutStream);
    getCardHolderNameTest();
    getCardExpiryDateTest();
    getCardPANTest();
    ST_terminalData_t terminalData;
    getTransactionDate(&terminalData);
    fprintf(myOutStream,"%s\n",terminalData.transactionDate);
    return 0;
}

void appStart(void){
    ST_cardData_t cardData;
    ST_terminalData_t terminalData;
    ST_transaction_t transactionData;

    getCardHolderName(&cardData);
    getCardExpiryDate(&cardData);
    getCardPAN(&cardData);
    transactionData.cardHolderData = cardData;

    getTransactionDate(&terminalData);
    if(isCardExpired(&cardData,&terminalData) != EXPIRED_CARD){
        getTransactionAmount(&terminalData);
        if(isBelowMaxAmount(&terminalData) != EXCEED_MAX_AMOUNT){
            
        }

    }
}