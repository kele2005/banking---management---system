#include "customer_manager.h"
#include "globals.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <stdexcept>

// ─── Constructor ──────────────────────────────────────────────────────────────
CustomerManager::CustomerManager(TransactionManager& txnMgr, BranchManager& branchMgr)
    : txnMgr_(txnMgr), branchMgr_(branchMgr) {}

// ─── File I/O ─────────────────────────────────────────────────────────────────
bool CustomerManager::loadCustomers() {
    std::ifstream file(FILE_CUSTOMERS, std::ios::binary);
    if (!file.is_open()) return false;

    accounts_.clear();
    Account a;
    while (file.read(reinterpret_cast<char*>(&a), sizeof(Account))) {
        accounts_.push_back(a);
    }
    file.close();
    return true;
}

bool CustomerManager::saveCustomers() {
    std::ofstream file(FILE_CUSTOMERS, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) return false;

    for (const auto& a : accounts_) {
        file.write(reinterpret_cast<const char*>(&a), sizeof(Account));
    }
    file.close();
    return true;
}

// ─── Sequence Counter ─────────────────────────────────────────────────────────
int CustomerManager::getNextSequence(const std::string& branchCode) {
    for (auto& p : sequences_) {
        if (p.first == branchCode) {
            return ++p.second;
        }
    }
    // Count existing accounts for this branch to determine starting sequence
    int count = 1;
    for (const auto& a : accounts_) {
        if (std::string(a.branchCode) == branchCode) ++count;
    }
    sequences_.push_back({branchCode, count});
    return count;
}

// ─── Input Helpers ────────────────────────────────────────────────────────────
std::string CustomerManager::inputValidatedSAID() const {
    std::string id;
    while (true) {
        std::cout << "SA ID Number (13 digits): ";
        std::getline(std::cin, id);
        id = Utils::trim(id);
        if (Utils::validateSAID(id)) return id;
        std::cout << "  [!] Invalid SA ID. Please try again.\n";
    }
}

std::string CustomerManager::inputValidatedEmail() const {
    std::string email;
    while (true) {
        std::cout << "Email Address: ";
        std::getline(std::cin, email);
        email = Utils::trim(email);
        if (Utils::validateEmail(email)) return email;
        std::cout << "  [!] Invalid email. Must contain @ and domain.\n";
    }
}

std::string CustomerManager::inputValidatedPhone() const {
    std::string phone;
    while (true) {
        std::cout << "Contact Number (10 digits): ";
        std::getline(std::cin, phone);
        phone = Utils::trim(phone);
        if (Utils::validatePhone(phone)) return phone;
        std::cout << "  [!] Invalid phone. Must be exactly 10 digits.\n";
    }
}

std::string CustomerManager::inputValidatedDate() const {
    std::string date;
    while (true) {
        std::cout << "Date of Birth (DD/MM/YYYY): ";
        std::getline(std::cin, date);
        date = Utils::trim(date);
        if (Utils::validateDate(date)) return date;
        std::cout << "  [!] Invalid date format. Use DD/MM/YYYY.\n";
    }
}

int CustomerManager::inputAccountType() const {
    int choice = 0;
    while (true) {
        std::cout << "Account Type:\n";
        std::cout << "  1. Savings   (min R" << MIN_SAVINGS << ")\n";
        std::cout << "  2. Cheque    (min R" << MIN_CHEQUE  << ")\n";
        std::cout << "  3. Fixed Deposit (min R" << MIN_FIXED << ")\n";
        std::cout << "  4. Student   (min R" << MIN_STUDENT << ")\n";
        std::cout << "Choice: ";
        std::string line;
        std::getline(std::cin, line);
        try {
            choice = std::stoi(line);
            if (choice >= 1 && choice <= 4) return choice;
        } catch (...) {}
        std::cout << "  [!] Enter 1-4.\n";
    }
}

double CustomerManager::inputInitialDeposit(int accType) const {
    double minDep = 0;
    switch (accType) {
        case SAVINGS:       minDep = MIN_SAVINGS; break;
        case CHEQUE:        minDep = MIN_CHEQUE;  break;
        case FIXED_DEPOSIT: minDep = MIN_FIXED;   break;
        case STUDENT:       minDep = MIN_STUDENT; break;
    }
    double amount = 0;
    while (true) {
        std::cout << "Initial Deposit (minimum R" << std::fixed
                  << std::setprecision(2) << minDep << "): R";
        std::string line;
        std::getline(std::cin, line);
        try {
            amount = std::stod(line);
            if (amount >= minDep) return amount;
        } catch (...) {}
        std::cout << "  [!] Amount must be at least R" << minDep << ".\n";
    }
}

// ─── Register Customer ────────────────────────────────────────────────────────
std::string CustomerManager::registerCustomer(const std::string& branchCode,
                                              const std::string& tellerID) {
    Utils::printBanner("REGISTER NEW CUSTOMER");

    Account acc;

    // First Name
    std::cout << "First Name: ";
    std::string firstName;
    std::getline(std::cin, firstName);
    firstName = Utils::trim(firstName);
    std::strncpy(acc.firstName, firstName.c_str(), sizeof(acc.firstName)-1);

    // Surname
    std::cout << "Surname: ";
    std::string surname;
    std::getline(std::cin, surname);
    surname = Utils::trim(surname);
    std::strncpy(acc.surname, surname.c_str(), sizeof(acc.surname)-1);

    // SA ID
    std::string saID = inputValidatedSAID();
    std::strncpy(acc.saID, saID.c_str(), sizeof(acc.saID)-1);

    // Contact
    std::string phone = inputValidatedPhone();
    std::strncpy(acc.contactNumber, phone.c_str(), sizeof(acc.contactNumber)-1);

    // Email
    std::string email = inputValidatedEmail();
    std::strncpy(acc.email, email.c_str(), sizeof(acc.email)-1);

    // Address
    std::cout << "Physical Address: ";
    std::string address;
    std::getline(std::cin, address);
    address = Utils::trim(address);
    std::strncpy(acc.address, address.c_str(), sizeof(acc.address)-1);

    // Date of Birth
    std::string dob = inputValidatedDate();
    std::strncpy(acc.dateOfBirth, dob.c_str(), sizeof(acc.dateOfBirth)-1);

    // Account Type
    acc.accountType = inputAccountType();

    // Initial Deposit
    double deposit = inputInitialDeposit(acc.accountType);
    acc.balance = deposit;

    // Branch
    std::strncpy(acc.branchCode, branchCode.c_str(), sizeof(acc.branchCode)-1);

    // Auto-generate account number
    int seq = getNextSequence(branchCode);
    std::string accNum = Utils::generateAccountNumber(branchCode, seq);
    std::strncpy(acc.accountNumber, accNum.c_str(), sizeof(acc.accountNumber)-1);

    // Auto-generate PIN
    std::string pin = Utils::generatePIN();
    acc.setPIN(pin);

    // Date opened
    std::string ts = Utils::currentTimestamp();
    std::strncpy(acc.dateOpened, ts.c_str(), sizeof(acc.dateOpened)-1);

    acc.isActive      = true;
    acc.isLocked      = false;
    acc.loginAttempts = 0;

    accounts_.push_back(acc);
    saveCustomers();

    // Record initial deposit transaction
    Transaction t;
    std::strncpy(t.accountNumber, accNum.c_str(), sizeof(t.accountNumber)-1);
    std::strncpy(t.branchCode,    branchCode.c_str(), sizeof(t.branchCode)-1);
    std::strncpy(t.tellerID,      tellerID.c_str(), sizeof(t.tellerID)-1);
    t.type          = DEPOSIT;
    t.amount        = deposit;
    t.balanceBefore = 0.0;
    t.balanceAfter  = deposit;
    std::strncpy(t.description, "Initial deposit on account opening", sizeof(t.description)-1);
    txnMgr_.recordTransaction(t);

    // Update branch stats
    branchMgr_.updateStats(branchCode, deposit, 0.0, 1);

    std::cout << "\n";
    Utils::printDivider('*');
    std::cout << "  Account created successfully!\n";
    std::cout << "  Account Number : " << accNum << "\n";
    std::cout << "  Customer PIN   : " << pin << "  (Display once - note it down!)\n";
    Utils::printDivider('*');

    return pin;
}

// ─── View Customer Details ────────────────────────────────────────────────────
void CustomerManager::viewCustomerDetails(const std::string& accountNumber) const {
    const Account* acc = findByAccountNumber(accountNumber);
    if (!acc) {
        std::cout << "Account not found: " << accountNumber << "\n";
        return;
    }
    Utils::printBanner("CUSTOMER DETAILS");
    std::cout << std::left;
    std::cout << std::setw(20) << "Account Number" << ": " << acc->accountNumber << "\n";
    std::cout << std::setw(20) << "Full Name"      << ": " << acc->getFullName() << "\n";
    std::cout << std::setw(20) << "SA ID"          << ": " << acc->saID          << "\n";
    std::cout << std::setw(20) << "Contact"        << ": " << acc->contactNumber << "\n";
    std::cout << std::setw(20) << "Email"          << ": " << acc->email         << "\n";
    std::cout << std::setw(20) << "Address"        << ": " << acc->address       << "\n";
    std::cout << std::setw(20) << "Date of Birth"  << ": " << acc->dateOfBirth   << "\n";
    std::cout << std::setw(20) << "Account Type"   << ": " << acc->typeLabel()   << "\n";
    std::cout << std::setw(20) << "Branch"         << ": " << acc->branchCode    << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::setw(20) << "Balance"        << ": R" << acc->balance      << "\n";
    std::cout << std::setw(20) << "Date Opened"    << ": " << acc->dateOpened    << "\n";
    std::cout << std::setw(20) << "Status"         << ": "
              << (acc->isActive ? (acc->isLocked ? "LOCKED" : "Active") : "Closed") << "\n";
    Utils::printDivider('-');
}

// ─── Customer Login ───────────────────────────────────────────────────────────
Account* CustomerManager::customerLogin(const std::string& accountNumber,
                                        const std::string& pin) {
    Account* acc = findByAccountNumber(accountNumber);
    if (!acc) return nullptr;
    if (!acc->isActive) {
        std::cout << "  [!] Account is closed.\n";
        return nullptr;
    }
    if (acc->isLocked) {
        std::cout << "  [!] Account is temporarily locked. Please visit a branch.\n";
        return nullptr;
    }
    if (acc->verifyPIN(pin)) {
        acc->loginAttempts = 0;
        saveCustomers();
        return acc;
    }
    // Failed attempt
    acc->loginAttempts++;
    if (acc->loginAttempts >= MAX_LOGIN_ATTEMPTS) {
        acc->isLocked = true;
        std::cout << "  [!] Too many failed attempts. Account locked.\n";
    } else {
        std::cout << "  [!] Incorrect PIN. Attempts remaining: "
                  << (MAX_LOGIN_ATTEMPTS - acc->loginAttempts) << "\n";
    }
    saveCustomers();
    return nullptr;
}

// ─── View Balance ─────────────────────────────────────────────────────────────
void CustomerManager::viewBalance(const Account* acc) const {
    Utils::printBanner("ACCOUNT BALANCE");
    std::cout << "Account : " << acc->accountNumber << "\n";
    std::cout << "Name    : " << acc->getFullName()  << "\n";
    std::cout << "Type    : " << acc->typeLabel()    << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Balance : R" << acc->balance       << "\n";
    Utils::printDivider('-');
}

// ─── Deposit ──────────────────────────────────────────────────────────────────
bool CustomerManager::deposit(Account* acc, double amount, const std::string& tellerID) {
    try {
        if (amount <= 0) throw std::invalid_argument("Deposit amount must be positive.");
        if (!acc->isActive) throw std::runtime_error("Account is not active.");

        double before = acc->balance;
        acc->balance += amount;

        Transaction t;
        std::strncpy(t.accountNumber, acc->accountNumber, sizeof(t.accountNumber)-1);
        std::strncpy(t.branchCode,    acc->branchCode,    sizeof(t.branchCode)-1);
        std::strncpy(t.tellerID,      tellerID.c_str(),   sizeof(t.tellerID)-1);
        t.type          = DEPOSIT;
        t.amount        = amount;
        t.balanceBefore = before;
        t.balanceAfter  = acc->balance;
        std::strncpy(t.description, "Cash deposit", sizeof(t.description)-1);
        txnMgr_.recordTransaction(t);

        branchMgr_.updateStats(std::string(acc->branchCode), amount, 0.0, 0);
        saveCustomers();

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  Deposit of R" << amount << " successful.\n";
        std::cout << "  New balance: R" << acc->balance << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "  [!] Deposit failed: " << e.what() << "\n";
        return false;
    }
}

// ─── Withdrawal ───────────────────────────────────────────────────────────────
bool CustomerManager::withdraw(Account* acc, double amount, const std::string& tellerID) {
    try {
        if (amount <= 0) throw std::invalid_argument("Withdrawal amount must be positive.");
        if (!acc->isActive) throw std::runtime_error("Account is not active.");

        // Cheque accounts may use overdraft
        double effectiveBalance = acc->balance;
        if (acc->accountType == CHEQUE) {
            effectiveBalance += 1000.0; // overdraft limit
        }
        if (amount > effectiveBalance) {
            throw std::runtime_error("Insufficient funds.");
        }
        if (acc->accountType == FIXED_DEPOSIT) {
            throw std::runtime_error("Withdrawals not permitted on Fixed Deposit accounts before maturity.");
        }

        double before = acc->balance;
        acc->balance -= amount;

        Transaction t;
        std::strncpy(t.accountNumber, acc->accountNumber, sizeof(t.accountNumber)-1);
        std::strncpy(t.branchCode,    acc->branchCode,    sizeof(t.branchCode)-1);
        std::strncpy(t.tellerID,      tellerID.c_str(),   sizeof(t.tellerID)-1);
        t.type          = WITHDRAWAL;
        t.amount        = amount;
        t.balanceBefore = before;
        t.balanceAfter  = acc->balance;
        std::strncpy(t.description, "Cash withdrawal", sizeof(t.description)-1);
        txnMgr_.recordTransaction(t);

        branchMgr_.updateStats(std::string(acc->branchCode), 0.0, amount, 0);
        saveCustomers();

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  Withdrawal of R" << amount << " successful.\n";
        std::cout << "  New balance: R" << acc->balance << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "  [!] Withdrawal failed: " << e.what() << "\n";
        return false;
    }
}

// ─── Transfer ─────────────────────────────────────────────────────────────────
bool CustomerManager::transfer(Account* src, const std::string& destAccNum,
                               double amount, const std::string& tellerID) {
    try {
        if (amount <= 0) throw std::invalid_argument("Transfer amount must be positive.");
        if (std::string(src->accountNumber) == destAccNum)
            throw std::invalid_argument("Cannot transfer to the same account.");

        Account* dest = findByAccountNumber(destAccNum);
        if (!dest) throw std::runtime_error("Destination account not found.");
        if (!dest->isActive) throw std::runtime_error("Destination account is not active.");

        if (amount > src->balance) throw std::runtime_error("Insufficient funds.");

        double srcBefore  = src->balance;
        double destBefore = dest->balance;

        src->balance  -= amount;
        dest->balance += amount;

        // Record debit on source
        Transaction t1;
        std::strncpy(t1.accountNumber, src->accountNumber,  sizeof(t1.accountNumber)-1);
        std::strncpy(t1.branchCode,    src->branchCode,     sizeof(t1.branchCode)-1);
        std::strncpy(t1.tellerID,      tellerID.c_str(),    sizeof(t1.tellerID)-1);
        std::strncpy(t1.targetAccount, destAccNum.c_str(),  sizeof(t1.targetAccount)-1);
        t1.type          = TRANSFER;
        t1.amount        = amount;
        t1.balanceBefore = srcBefore;
        t1.balanceAfter  = src->balance;
        std::string desc1 = "Transfer to " + destAccNum;
        std::strncpy(t1.description, desc1.c_str(), sizeof(t1.description)-1);
        txnMgr_.recordTransaction(t1);

        // Record credit on destination
        Transaction t2;
        std::strncpy(t2.accountNumber, dest->accountNumber, sizeof(t2.accountNumber)-1);
        std::strncpy(t2.branchCode,    dest->branchCode,    sizeof(t2.branchCode)-1);
        std::strncpy(t2.tellerID,      tellerID.c_str(),    sizeof(t2.tellerID)-1);
        std::strncpy(t2.targetAccount, src->accountNumber,  sizeof(t2.targetAccount)-1);
        t2.type          = DEPOSIT;
        t2.amount        = amount;
        t2.balanceBefore = destBefore;
        t2.balanceAfter  = dest->balance;
        std::string desc2 = "Transfer from " + std::string(src->accountNumber);
        std::strncpy(t2.description, desc2.c_str(), sizeof(t2.description)-1);
        txnMgr_.recordTransaction(t2);

        saveCustomers();

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  Transfer of R" << amount << " to " << destAccNum << " successful.\n";
        std::cout << "  Your new balance: R" << src->balance << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "  [!] Transfer failed: " << e.what() << "\n";
        return false;
    }
}

// ─── Change PIN ───────────────────────────────────────────────────────────────
bool CustomerManager::changePIN(Account* acc, const std::string& oldPIN,
                                const std::string& newPIN) {
    try {
        if (!acc->verifyPIN(oldPIN))
            throw std::runtime_error("Current PIN is incorrect.");
        if (newPIN.length() != PIN_LENGTH)
            throw std::invalid_argument("New PIN must be exactly 5 digits.");
        for (char c : newPIN) {
            if (!std::isdigit(static_cast<unsigned char>(c)))
                throw std::invalid_argument("PIN must contain digits only.");
        }
        if (oldPIN == newPIN)
            throw std::invalid_argument("New PIN must differ from current PIN.");

        acc->setPIN(newPIN);
        saveCustomers();
        std::cout << "  PIN changed successfully.\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "  [!] PIN change failed: " << e.what() << "\n";
        return false;
    }
}

// ─── Search Functions ─────────────────────────────────────────────────────────
Account* CustomerManager::findByAccountNumber(const std::string& accNum) {
    for (auto& a : accounts_) {
        if (std::string(a.accountNumber) == accNum) return &a;
    }
    return nullptr;
}

const Account* CustomerManager::findByAccountNumber(const std::string& accNum) const {
    for (const auto& a : accounts_) {
        if (std::string(a.accountNumber) == accNum) return &a;
    }
    return nullptr;
}

std::vector<Account*> CustomerManager::searchByName(const std::string& name) {
    std::string upper = Utils::toUpper(name);
    std::vector<Account*> results;
    for (auto& a : accounts_) {
        std::string fullName = Utils::toUpper(a.getFullName());
        if (fullName.find(upper) != std::string::npos) {
            results.push_back(&a);
        }
    }
    return results;
}

std::vector<Account*> CustomerManager::searchBySAID(const std::string& saID) {
    std::vector<Account*> results;
    for (auto& a : accounts_) {
        if (std::string(a.saID) == saID) results.push_back(&a);
    }
    return results;
}

std::vector<Account*> CustomerManager::getAccountsByBranch(const std::string& branchCode) {
    std::vector<Account*> results;
    for (auto& a : accounts_) {
        if (std::string(a.branchCode) == branchCode) results.push_back(&a);
    }
    return results;
}

// ─── Interest Calculation ─────────────────────────────────────────────────────
void CustomerManager::applyMonthlyInterest(const std::string& branchCode) {
    int count = 0;
    double totalInterest = 0;

    for (auto& a : accounts_) {
        if (!a.isActive) continue;
        if (!branchCode.empty() && std::string(a.branchCode) != branchCode) continue;
        if (a.accountType == CHEQUE) continue; // Cheque earns minimal interest, skip for simplicity

        double rate    = a.interestRate();
        double monthly = (rate / 100.0 / 12.0) * a.balance;
        if (monthly <= 0) continue;

        double before = a.balance;
        a.balance += monthly;
        totalInterest += monthly;
        ++count;

        Transaction t;
        std::strncpy(t.accountNumber, a.accountNumber, sizeof(t.accountNumber)-1);
        std::strncpy(t.branchCode,    a.branchCode,    sizeof(t.branchCode)-1);
        t.type          = INTEREST;
        t.amount        = monthly;
        t.balanceBefore = before;
        t.balanceAfter  = a.balance;
        std::strncpy(t.description, "Monthly interest applied", sizeof(t.description)-1);
        txnMgr_.recordTransaction(t);
    }

    saveCustomers();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Interest applied to " << count << " accounts. Total: R" << totalInterest << "\n";
}

// ─── Reports ──────────────────────────────────────────────────────────────────
void CustomerManager::printAccountSummaryReport(const std::string& branchCode) const {
    Utils::printBanner("CUSTOMER ACCOUNT SUMMARY");
    if (!branchCode.empty()) std::cout << "Branch: " << branchCode << "\n";

    std::cout << std::left
              << std::setw(22) << "Account Number"
              << std::setw(30) << "Name"
              << std::setw(16) << "Type"
              << std::setw(14) << "Balance"
              << "Status\n";
    Utils::printDivider('-');

    double total = 0;
    int    cnt   = 0;
    for (const auto& a : accounts_) {
        if (!branchCode.empty() && std::string(a.branchCode) != branchCode) continue;
        std::cout << std::left
                  << std::setw(22) << a.accountNumber
                  << std::setw(30) << a.getFullName()
                  << std::setw(16) << a.typeLabel()
                  << std::setw(14) << std::fixed << std::setprecision(2) << a.balance
                  << (a.isActive ? (a.isLocked ? "LOCKED" : "Active") : "Closed") << "\n";
        total += a.balance;
        ++cnt;
    }
    Utils::printDivider('-');
    std::cout << "Total accounts: " << cnt << "   Total balance: R"
              << std::fixed << std::setprecision(2) << total << "\n";
}

void CustomerManager::printBranchPerformanceReport() const {
    Utils::printBanner("BRANCH PERFORMANCE REPORT");
    const auto& branches = branchMgr_.getBranches();

    std::cout << std::left
              << std::setw(8)  << "Code"
              << std::setw(25) << "Branch"
              << std::setw(12) << "Accounts"
              << std::setw(18) << "Total Balance"
              << std::setw(18) << "Deposits"
              << "Withdrawals\n";
    Utils::printDivider('-');

    for (const auto& b : branches) {
        double totalBal = 0;
        for (const auto& a : accounts_) {
            if (std::string(a.branchCode) == std::string(b.code)) {
                totalBal += a.balance;
            }
        }
        std::cout << std::left
                  << std::setw(8)  << b.code
                  << std::setw(25) << b.name
                  << std::setw(12) << b.totalAccounts
                  << std::setw(18) << std::fixed << std::setprecision(2) << totalBal
                  << std::setw(18) << b.totalDeposits
                  << b.totalWithdrawals << "\n";
    }
    Utils::printDivider('-');
}

// ─── Backup / Export ──────────────────────────────────────────────────────────
bool CustomerManager::backupData() const {
    std::string ts = Utils::currentTimestamp();
    // Replace spaces and colons for filename safety
    for (char& c : ts) {
        if (c == ' ' || c == ':') c = '_';
    }
    std::string backupFile = BACKUP_DIR + "customers_" + ts + ".dat";
    std::ifstream src(FILE_CUSTOMERS, std::ios::binary);
    std::ofstream dst(backupFile,     std::ios::binary);
    if (!src.is_open() || !dst.is_open()) return false;
    dst << src.rdbuf();
    std::cout << "  Backup saved to: " << backupFile << "\n";
    return true;
}

bool CustomerManager::exportCustomersToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "AccountNumber,FirstName,Surname,SAID,Contact,Email,Address,"
         << "DateOfBirth,AccountType,Branch,Balance,DateOpened,Status\n";

    for (const auto& a : accounts_) {
        file << a.accountNumber << ","
             << a.firstName     << ","
             << a.surname       << ","
             << a.saID          << ","
             << a.contactNumber << ","
             << a.email         << ","
             << "\"" << a.address << "\","
             << a.dateOfBirth   << ","
             << a.typeLabel()   << ","
             << a.branchCode    << ","
             << std::fixed << std::setprecision(2) << a.balance << ","
             << a.dateOpened    << ","
             << (a.isActive ? "Active" : "Closed") << "\n";
    }
    file.close();
    return true;
}
