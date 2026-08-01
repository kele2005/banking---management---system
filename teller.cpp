#include "account.h"
#include "globals.h"
#include "utils.h"
#include <cstring>

// ─── Base Account ─────────────────────────────────────────────────────────────
Account::Account() {
    std::memset(accountNumber, 0, sizeof(accountNumber));
    std::memset(firstName,     0, sizeof(firstName));
    std::memset(surname,       0, sizeof(surname));
    std::memset(saID,          0, sizeof(saID));
    std::memset(contactNumber, 0, sizeof(contactNumber));
    std::memset(email,         0, sizeof(email));
    std::memset(address,       0, sizeof(address));
    std::memset(dateOfBirth,   0, sizeof(dateOfBirth));
    std::memset(branchCode,    0, sizeof(branchCode));
    std::memset(encryptedPIN,  0, sizeof(encryptedPIN));
    std::memset(dateOpened,    0, sizeof(dateOpened));
    accountType    = SAVINGS;
    balance        = 0.0;
    isLocked       = false;
    loginAttempts  = 0;
    isActive       = true;
}

double Account::minimumDeposit() const {
    switch (accountType) {
        case SAVINGS:       return MIN_SAVINGS;
        case CHEQUE:        return MIN_CHEQUE;
        case FIXED_DEPOSIT: return MIN_FIXED;
        case STUDENT:       return MIN_STUDENT;
        default:            return 0.0;
    }
}

double Account::interestRate() const {
    switch (accountType) {
        case SAVINGS:       return RATE_SAVINGS;
        case CHEQUE:        return RATE_CHEQUE;
        case FIXED_DEPOSIT: return RATE_FIXED;
        case STUDENT:       return RATE_STUDENT;
        default:            return 0.0;
    }
}

std::string Account::typeLabel() const {
    return Utils::accountTypeStr(accountType);
}

bool Account::verifyPIN(const std::string& plainPIN) const {
    std::string enc = Utils::encrypt(plainPIN);
    return enc == std::string(encryptedPIN);
}

void Account::setPIN(const std::string& plainPIN) {
    std::string enc = Utils::encrypt(plainPIN);
    std::strncpy(encryptedPIN, enc.c_str(), sizeof(encryptedPIN)-1);
}

std::string Account::getFullName() const {
    return std::string(firstName) + " " + std::string(surname);
}

// ─── SavingsAccount ───────────────────────────────────────────────────────────
SavingsAccount::SavingsAccount() : Account() {
    accountType = SAVINGS;
}
double SavingsAccount::minimumDeposit() const { return MIN_SAVINGS; }
double SavingsAccount::interestRate()   const { return RATE_SAVINGS; }
std::string SavingsAccount::typeLabel() const { return "Savings"; }

// ─── ChequeAccount ────────────────────────────────────────────────────────────
ChequeAccount::ChequeAccount() : Account() {
    accountType   = CHEQUE;
    overdraftLimit = 1000.0;
}
double ChequeAccount::minimumDeposit() const { return MIN_CHEQUE; }
double ChequeAccount::interestRate()   const { return RATE_CHEQUE; }
std::string ChequeAccount::typeLabel() const { return "Cheque"; }

// ─── FixedDepositAccount ──────────────────────────────────────────────────────
FixedDepositAccount::FixedDepositAccount() : Account() {
    accountType = FIXED_DEPOSIT;
    termMonths  = 12;
    std::memset(maturityDate, 0, sizeof(maturityDate));
}
double FixedDepositAccount::minimumDeposit() const { return MIN_FIXED; }
double FixedDepositAccount::interestRate()   const { return RATE_FIXED; }
std::string FixedDepositAccount::typeLabel() const { return "Fixed Deposit"; }

// ─── StudentAccount ───────────────────────────────────────────────────────────
StudentAccount::StudentAccount() : Account() {
    accountType = STUDENT;
    std::memset(institution,   0, sizeof(institution));
    std::memset(studentNumber, 0, sizeof(studentNumber));
}
double StudentAccount::minimumDeposit() const { return MIN_STUDENT; }
double StudentAccount::interestRate()   const { return RATE_STUDENT; }
std::string StudentAccount::typeLabel() const { return "Student"; }
