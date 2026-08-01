#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include "globals.h"

// ─── Base Account Class ───────────────────────────────────────────────────────
class Account {
public:
    char   accountNumber[30];   // ACC-BRANCH-XXXXX
    char   firstName[40];
    char   surname[40];
    char   saID[14];            // 13-digit SA ID
    char   contactNumber[12];
    char   email[80];
    char   address[120];
    char   dateOfBirth[12];     // DD/MM/YYYY
    int    accountType;         // AccountType enum value
    char   branchCode[10];
    double balance;
    char   encryptedPIN[16];    // 5-digit PIN, XOR-encrypted
    bool   isLocked;            // locked after 3 failed PIN attempts
    int    loginAttempts;
    bool   isActive;
    char   dateOpened[20];      // timestamp

    Account();

    // Virtual destructor for polymorphism
    virtual ~Account() {}

    // Returns minimum deposit for this account type
    virtual double minimumDeposit() const;

    // Returns annual interest rate for this account type
    virtual double interestRate() const;

    // Returns account type label
    virtual std::string typeLabel() const;

    bool verifyPIN(const std::string& plainPIN) const;
    void setPIN(const std::string& plainPIN);

    std::string getFullName() const;
    std::string getAccountNumber() const { return std::string(accountNumber); }
    std::string getBranchCode()    const { return std::string(branchCode); }
};

// ─── Derived Account Types ────────────────────────────────────────────────────

class SavingsAccount : public Account {
public:
    SavingsAccount();
    double minimumDeposit() const override;
    double interestRate()   const override;
    std::string typeLabel() const override;
};

class ChequeAccount : public Account {
public:
    double overdraftLimit;
    ChequeAccount();
    double minimumDeposit() const override;
    double interestRate()   const override;
    std::string typeLabel() const override;
};

class FixedDepositAccount : public Account {
public:
    int    termMonths;      // lock-in period
    char   maturityDate[20];
    FixedDepositAccount();
    double minimumDeposit() const override;
    double interestRate()   const override;
    std::string typeLabel() const override;
};

class StudentAccount : public Account {
public:
    char   institution[80];
    char   studentNumber[20];
    StudentAccount();
    double minimumDeposit() const override;
    double interestRate()   const override;
    std::string typeLabel() const override;
};

#endif // ACCOUNT_H
