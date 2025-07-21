//
// Created by iqbal on 2025-06-10.
//

#ifndef BASEACCOUNTCLASS_H
#define BASEACCOUNTCLASS_H

class BankAccount {
protected: char name[MAX_NAME_LEN]; //username
    char password[MAX_NAME_LEN];
    int account_number;
    double balance;
public: BankAccount();
    BankAccount(char * , char * , int, double);
    BankAccount(char * );
    BankAccount(char * , int, double);

    void printSummary();
    double withdraw(double);
    void deposit(double);
    double getBalance();
    void setName(char * );
    void setAccount_number(int);
    void setBalance(double);

    char * getUsername() { //const char *getUsername() const{
        return name;
    }
    bool checkPassword(char * input) {
        return strcmp(password, input) == 0;
    }
};

#endif //BASEACCOUNTCLASS_H
