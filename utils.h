#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <vector>
#include <ctime>

// ─── Constants ───────────────────────────────────────────────────────────────
const int MAX_LOGIN_ATTEMPTS   = 3;
const int PIN_LENGTH           = 5;
const int TELLER_ID_LEN        = 4;   // e.g. T001
const int MAX_TRANSACTIONS     = 1000;

// Minimum initial deposits per account type
const double MIN_SAVINGS       = 100.0;
const double MIN_CHEQUE        = 500.0;
const double MIN_FIXED         = 1000.0;
const double MIN_STUDENT       = 50.0;

// Interest rates (annual %)
const double RATE_SAVINGS      = 4.5;
const double RATE_CHEQUE       = 1.5;
const double RATE_FIXED        = 8.0;
const double RATE_STUDENT      = 2.0;

// File paths
const std::string FILE_TELLERS      = "data/tellers.dat";
const std::string FILE_CUSTOMERS    = "data/customers.dat";
const std::string FILE_TRANSACTIONS = "data/transactions.dat";
const std::string FILE_BRANCHES     = "data/branches.dat";
const std::string FILE_CONFIG       = "data/system_config.txt";
const std::string BACKUP_DIR        = "data/backup/";

// Account type codes
enum AccountType { SAVINGS = 1, CHEQUE = 2, FIXED_DEPOSIT = 3, STUDENT = 4 };

// Transaction type codes
enum TransactionType { DEPOSIT = 1, WITHDRAWAL = 2, TRANSFER = 3, INTEREST = 4 };

#endif // GLOBALS_H
