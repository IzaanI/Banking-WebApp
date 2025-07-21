//
// Created by iqbal on 2025-06-10.
//

#ifndef SAVEBANKACCOUNTCLASS_H
#define SAVEBANKACCOUNTCLASS_H
#include "baseAccountClass.h"

// The 2nd-level class: saveBankAccount
class saveBankAccount: public BankAccount { //inheritance of BankAccount
protected: double interestRate;
    int MAXnoWithDraws;
    int noWithDraws;

public: saveBankAccount(); // add a default constructor
    saveBankAccount(const char * name0,
      const char * password0, int acc_num0, double balance0, double interestRate0, int MAXnoWithdraws0);

    void printSummary();
    double withdraw(double);

    void callInterest();
    void resetWithdraws();
    void transfer(saveBankAccount & acc2, double);
    //void transactionHandler(int, vector<saveBankAccount>& accounts);
};

#endif //SAVEBANKACCOUNTCLASS_H
