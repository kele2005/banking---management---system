#ifndef CUSTOMER_MANAGER_H
#define CUSTOMER_MANAGER_H

#include <string>
#include <vector>
#include "account.h"
#include "transaction.h"
#include "branch.h"

// ─── Customer / Account Manager ───────────────────────────────────────────────
// Owns all Account records and coordinates with TransactionManager.
class CustomerManager {
public:
    CustomerManager(TransactionManager& txnMgr, BranchManager& branchMgr);

    bool loadCustomers();
    bool saveCustomers();

    // ── Teller functions ──────────────────────────────────────────────────────
    // Register a new customer; returns generated PIN (displayed once)
    std::string registerCustomer(const std::string& branchCode,
                                 const std::string& tellerID);

    void viewCustomerDetails(const std::string& accountNumber) const;

    // ── Customer self-service ─────────────────────────────────────────────────
    // Returns pointer to account if PIN correct, nullptr otherwise
    Account* customerLogin(const std::string& accountNumber,
                           const std::string& pin);

    void viewBalance(const Account* acc) const;
    bool deposit(Account* acc, double amount, const std::string& tellerID = "");
    bool withdraw(Account* acc, double amount, const std::string& tellerID = "");
    bool transfer(Account* src, const std::string& destAccNum, double amount,
                  const std::string& tellerID = "");
    bool changePIN(Account* acc, const std::string& oldPIN,
                   const std::string& newPIN);

    // ── Search ────────────────────────────────────────────────────────────────
    Account* findByAccountNumber(const std::string& accNum);
    const Account* findByAccountNumber(const std::string& accNum) const;
    std::vector<Account*> searchByName(const std::string& name);
    std::vector<Account*> searchBySAID(const std::string& saID);
    std::vector<Account*> getAccountsByBranch(const std::string& branchCode);

    // ── Interest ──────────────────────────────────────────────────────────────
    void applyMonthlyInterest(const std::string& branchCode = "");

    // ── Reports ───────────────────────────────────────────────────────────────
    void printAccountSummaryReport(const std::string& branchCode = "") const;
    void printBranchPerformanceReport() const;

    // ── Backup / Export ───────────────────────────────────────────────────────
    bool backupData() const;
    bool exportCustomersToCSV(const std::string& filename) const;

    int getNextSequence(const std::string& branchCode);

    const std::vector<Account>& getAccounts() const { return accounts_; }

private:
    std::vector<Account>     accounts_;
    TransactionManager&      txnMgr_;
    BranchManager&           branchMgr_;

    // Per-branch sequence counters for account number generation
    // stored as branchCode -> last sequence used
    std::vector<std::pair<std::string,int>> sequences_;

    // Input helpers used during registration
    std::string inputValidatedSAID()   const;
    std::string inputValidatedEmail()  const;
    std::string inputValidatedPhone()  const;
    std::string inputValidatedDate()   const;
    int         inputAccountType()     const;
    double      inputInitialDeposit(int accType) const;
};

#endif // CUSTOMER_MANAGER_H
