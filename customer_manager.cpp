#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>
#include "globals.h"

// ─── Transaction Record ───────────────────────────────────────────────────────
struct Transaction {
    char   transactionID[20];
    char   accountNumber[30];
    char   branchCode[10];
    int    type;              // TransactionType enum
    double amount;
    double balanceBefore;
    double balanceAfter;
    char   description[100];
    char   timestamp[25];
    char   tellerID[10];      // blank if self-service
    char   targetAccount[30]; // for transfers

    Transaction();
};

// ─── Transaction Manager ──────────────────────────────────────────────────────
class TransactionManager {
public:
    TransactionManager();

    bool loadTransactions();
    bool saveTransactions();

    // Record a new transaction and persist
    bool recordTransaction(const Transaction& t);

    // Retrieve transactions for a specific account
    std::vector<Transaction> getAccountTransactions(const std::string& accountNumber) const;

    // Retrieve transactions for a specific branch
    std::vector<Transaction> getBranchTransactions(const std::string& branchCode) const;

    // Retrieve today's transactions
    std::vector<Transaction> getTodayTransactions() const;

    // Print a formatted account statement
    void printStatement(const std::string& accountNumber) const;

    // Print daily transaction report
    void printDailyReport() const;

    // Export transactions to CSV
    bool exportToCSV(const std::string& filename) const;

    const std::vector<Transaction>& getAll() const { return transactions_; }

private:
    std::vector<Transaction> transactions_;
    int nextID_;

    std::string generateTxnID();
};

#endif // TRANSACTION_H
