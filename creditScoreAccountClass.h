//
// Created by iqbal on 2025-06-10.
//

#ifndef CREDITSCOREACCOUNTCLASS_H
#define CREDITSCOREACCOUNTCLASS_H

#include "saveBankAccountClass.h"

// The 3rd-level class: CreditScoreAccount
class CreditScoreAccount: public saveBankAccount { //inherit from saveBankAccount
protected: int age;
    long income;
    bool home_ownership;
    int CreditScore;
    bool creditAccountActive = false;
    public: CreditScoreAccount() {
        strcpy(name,"");
        strcpy(password,"");
        account_number = 0;
        balance = 0;
        interestRate = 0;
        MAXnoWithDraws = 2;
        age = 0;
        income = 0;
        home_ownership = false;
    }
public: CreditScoreAccount(const char * name0, const char * password0, int acc_num0, double balance0,
    double interestRate0, int MAXnoWtdraws0, int age0, long income0, bool home0);

    void obtainCredentials();
    int CreditScoreReport();
    void setCreditScoreAccount();

    void transactionHandler(int, vector < CreditScoreAccount > & accounts);
};

#endif //CREDITSCOREACCOUNTCLASS_H
