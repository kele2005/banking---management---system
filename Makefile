#include "menu.h"
#include "globals.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

// ─── Constructor ──────────────────────────────────────────────────────────────
Menu::Menu(TellerManager& tellerMgr, CustomerManager& custMgr,
           TransactionManager& txnMgr, BranchManager& branchMgr)
    : tellerMgr_(tellerMgr), custMgr_(custMgr),
      txnMgr_(txnMgr), branchMgr_(branchMgr) {}

// ─── Run ──────────────────────────────────────────────────────────────────────
void Menu::run() {
    mainMenu();
}

// ─── Main Menu ────────────────────────────────────────────────────────────────
void Menu::mainMenu() {
    while (true) {
        Utils::clearScreen();
        Utils::printBanner("STANDARD BANK - MULTI-BRANCH BANKING SYSTEM");
        std::cout << "  1. Teller Login\n";
        std::cout << "  2. Customer Login\n";
        std::cout << "  3. View Branch Information\n";
        std::cout << "  0. Exit\n";
        Utils::printDivider('-');
        std::cout << "Select: ";

        std::string choice;
        std::getline(std::cin, choice);

        if      (choice == "1") tellerLoginFlow();
        else if (choice == "2") customerLoginFlow();
        else if (choice == "3") branchMenu();
        else if (choice == "0") {
            std::cout << "\nThank you for using Standard Bank. Goodbye!\n";
            break;
        } else {
            std::cout << "  [!] Invalid option.\n";
            Utils::pause();
        }
    }
}

// ─── Teller Login Flow ────────────────────────────────────────────────────────
void Menu::tellerLoginFlow() {
    Utils::clearScreen();
    Utils::printBanner("TELLER LOGIN");

    std::cout << "Teller ID: ";
    std::string id;
    std::getline(std::cin, id);
    id = Utils::trim(id);

    std::cout << "Password : ";
    std::string password;
    std::getline(std::cin, password);

    Teller* teller = tellerMgr_.authenticate(id, password);
    if (!teller) {
        std::cout << "\n  [!] Invalid Teller ID or Password.\n";
        Utils::pause();
        return;
    }

    std::cout << "\n  Welcome, " << teller->getName()
              << " [Branch: " << teller->getBranch() << "]\n";
    Utils::pause();
    tellerMenu(teller);
}

// ─── Teller Menu ─────────────────────────────────────────────────────────────
void Menu::tellerMenu(Teller* teller) {
    while (true) {
        Utils::clearScreen();
        Utils::printBanner("TELLER MENU");
        std::cout << "  Teller : " << teller->getName()
                  << "  |  Branch: " << teller->getBranch() << "\n";
        Utils::printDivider('-');
        std::cout << "  1. Register New Customer\n";
        std::cout << "  2. View Customer Details\n";
        std::cout << "  3. Process Transaction\n";
        std::cout << "  4. Search Customer\n";
        std::cout << "  5. Apply Monthly Interest\n";
        std::cout << "  6. Reports\n";
        std::cout << "  7. Branch Information\n";
        std::cout << "  0. Logout\n";
        Utils::printDivider('-');
        std::cout << "Select: ";

        std::string choice;
        std::getline(std::cin, choice);

        if      (choice == "1") tellerRegisterCustomer(teller);
        else if (choice == "2") tellerViewCustomer(teller);
        else if (choice == "3") tellerProcessTransaction(teller);
        else if (choice == "4") tellerSearchCustomer(teller);
        else if (choice == "5") tellerApplyInterest(teller);
        else if (choice == "6") tellerBranchReports(teller);
        else if (choice == "7") {
            branchMgr_.viewBranchDetails(teller->getBranch());
            Utils::pause();
        }
        else if (choice == "0") break;
        else {
            std::cout << "  [!] Invalid option.\n";
            Utils::pause();
        }
    }
}

void Menu::tellerRegisterCustomer(Teller* teller) {
    Utils::clearScreen();
    custMgr_.registerCustomer(teller->getBranch(), teller->getID());
    Utils::pause();
}

void Menu::tellerViewCustomer(Teller* teller) {
    Utils::clearScreen();
    Utils::printBanner("VIEW CUSTOMER");
    std::cout << "Enter Account Number (or press Enter to list branch accounts): ";
    std::string accNum;
    std::getline(std::cin, accNum);
    accNum = Utils::trim(accNum);

    if (accNum.empty()) {
        // List all accounts for this branch
        auto accounts = custMgr_.getAccountsByBranch(teller->getBranch());
        if (accounts.empty()) {
            std::cout << "  No accounts found for branch " << teller->getBranch() << ".\n";
        } else {
            std::cout << std::left
                      << std::setw(22) << "Account Number"
                      << std::setw(30) << "Name"
                      << std::setw(16) << "Type"
                      << "Balance\n";
            Utils::printDivider('-');
            for (const auto* a : accounts) {
                std::cout << std::left
                          << std::setw(22) << a->accountNumber
                          << std::setw(30) << a->getFullName()
                          << std::setw(16) << a->typeLabel()
                          << std::fixed << std::setprecision(2) << a->balance << "\n";
            }
        }
    } else {
        custMgr_.viewCustomerDetails(accNum);
    }
    Utils::pause();
}

void Menu::tellerProcessTransaction(Teller* teller) {
    Utils::clearScreen();
    Utils::printBanner("PROCESS TRANSACTION");

    std::cout << "Customer Account Number: ";
    std::string accNum;
    std::getline(std::cin, accNum);
    accNum = Utils::trim(accNum);

    Account* acc = custMgr_.findByAccountNumber(accNum);
    if (!acc) {
        std::cout << "  [!] Account not found.\n";
        Utils::pause();
        return;
    }

    // Teller must verify customer PIN
    std::cout << "Verify Customer PIN: ";
    std::string pin;
    std::getline(std::cin, pin);
    if (!acc->verifyPIN(pin)) {
        std::cout << "  [!] Incorrect PIN. Transaction denied.\n";
        Utils::pause();
        return;
    }

    std::cout << "\n  Customer: " << acc->getFullName() << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Balance : R" << acc->balance << "\n\n";

    std::cout << "  1. Deposit\n";
    std::cout << "  2. Withdrawal\n";
    std::cout << "  3. Transfer\n";
    std::cout << "  4. View Statement\n";
    std::cout << "  0. Cancel\n";
    std::cout << "Select: ";

    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
        std::cout << "Deposit Amount: R";
        std::string amtStr;
        std::getline(std::cin, amtStr);
        try {
            double amt = std::stod(amtStr);
            custMgr_.deposit(acc, amt, teller->getID());
        } catch (...) {
            std::cout << "  [!] Invalid amount.\n";
        }
    } else if (choice == "2") {
        std::cout << "Withdrawal Amount: R";
        std::string amtStr;
        std::getline(std::cin, amtStr);
        try {
            double amt = std::stod(amtStr);
            custMgr_.withdraw(acc, amt, teller->getID());
        } catch (...) {
            std::cout << "  [!] Invalid amount.\n";
        }
    } else if (choice == "3") {
        std::cout << "Destination Account Number: ";
        std::string dest;
        std::getline(std::cin, dest);
        dest = Utils::trim(dest);
        std::cout << "Transfer Amount: R";
        std::string amtStr;
        std::getline(std::cin, amtStr);
        try {
            double amt = std::stod(amtStr);
            custMgr_.transfer(acc, dest, amt, teller->getID());
        } catch (...) {
            std::cout << "  [!] Invalid amount.\n";
        }
    } else if (choice == "4") {
        txnMgr_.printStatement(accNum);
    }

    Utils::pause();
}

void Menu::tellerBranchReports(Teller* teller) {
    reportMenu(teller->getBranch());
}

void Menu::tellerApplyInterest(Teller* teller) {
    Utils::clearScreen();
    Utils::printBanner("APPLY MONTHLY INTEREST");
    std::cout << "  Apply interest to all accounts in branch " << teller->getBranch() << "? (y/n): ";
    std::string confirm;
    std::getline(std::cin, confirm);
    if (confirm == "y" || confirm == "Y") {
        custMgr_.applyMonthlyInterest(teller->getBranch());
    } else {
        std::cout << "  Cancelled.\n";
    }
    Utils::pause();
}

void Menu::tellerSearchCustomer(Teller* teller) {
    Utils::clearScreen();
    Utils::printBanner("SEARCH CUSTOMER");
    std::cout << "  1. Search by Name\n";
    std::cout << "  2. Search by SA ID\n";
    std::cout << "  3. Search by Account Number\n";
    std::cout << "Select: ";

    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
        std::cout << "Enter name (partial): ";
        std::string name;
        std::getline(std::cin, name);
        auto results = custMgr_.searchByName(Utils::trim(name));
        if (results.empty()) {
            std::cout << "  No results found.\n";
        } else {
            std::cout << "\nFound " << results.size() << " result(s):\n";
            Utils::printDivider('-');
            for (const auto* a : results) {
                std::cout << a->accountNumber << "  " << a->getFullName()
                          << "  [" << a->typeLabel() << "]  R"
                          << std::fixed << std::setprecision(2) << a->balance << "\n";
            }
        }
    } else if (choice == "2") {
        std::cout << "Enter SA ID: ";
        std::string saID;
        std::getline(std::cin, saID);
        auto results = custMgr_.searchBySAID(Utils::trim(saID));
        if (results.empty()) {
            std::cout << "  No results found.\n";
        } else {
            for (const auto* a : results) {
                custMgr_.viewCustomerDetails(std::string(a->accountNumber));
            }
        }
    } else if (choice == "3") {
        std::cout << "Enter Account Number: ";
        std::string accNum;
        std::getline(std::cin, accNum);
        custMgr_.viewCustomerDetails(Utils::trim(accNum));
    }

    Utils::pause();
}

// ─── Customer Login Flow ──────────────────────────────────────────────────────
void Menu::customerLoginFlow() {
    Utils::clearScreen();
    Utils::printBanner("CUSTOMER LOGIN");

    std::cout << "Account Number (ACC-BRANCH-XXXXX): ";
    std::string accNum;
    std::getline(std::cin, accNum);
    accNum = Utils::trim(accNum);

    std::cout << "5-digit PIN: ";
    std::string pin;
    std::getline(std::cin, pin);
    pin = Utils::trim(pin);

    Account* acc = custMgr_.customerLogin(accNum, pin);
    if (!acc) {
        Utils::pause();
        return;
    }

    std::cout << "\n  Welcome, " << acc->getFullName() << "!\n";
    Utils::pause();
    customerMenu(acc);
}

// ─── Customer Menu ────────────────────────────────────────────────────────────
void Menu::customerMenu(Account* acc) {
    while (true) {
        Utils::clearScreen();
        Utils::printBanner("CUSTOMER MENU");
        std::cout << "  Account: " << acc->accountNumber
                  << "  |  " << acc->getFullName() << "\n";
        Utils::printDivider('-');
        std::cout << "  1. View Balance\n";
        std::cout << "  2. Deposit\n";
        std::cout << "  3. Withdrawal\n";
        std::cout << "  4. Transfer\n";
        std::cout << "  5. Account Statement\n";
        std::cout << "  6. Change PIN\n";
        std::cout << "  0. Logout\n";
        Utils::printDivider('-');
        std::cout << "Select: ";

        std::string choice;
        std::getline(std::cin, choice);

        if      (choice == "1") { custMgr_.viewBalance(acc); Utils::pause(); }
        else if (choice == "2") customerDeposit(acc);
        else if (choice == "3") customerWithdraw(acc);
        else if (choice == "4") customerTransfer(acc);
        else if (choice == "5") customerStatement(acc);
        else if (choice == "6") customerChangePIN(acc);
        else if (choice == "0") break;
        else {
            std::cout << "  [!] Invalid option.\n";
            Utils::pause();
        }
    }
}

void Menu::customerDeposit(Account* acc) {
    Utils::clearScreen();
    Utils::printBanner("DEPOSIT");
    std::cout << "Current Balance: R" << std::fixed << std::setprecision(2) << acc->balance << "\n";
    std::cout << "Deposit Amount: R";
    std::string amtStr;
    std::getline(std::cin, amtStr);
    try {
        double amt = std::stod(amtStr);
        custMgr_.deposit(acc, amt);
    } catch (...) {
        std::cout << "  [!] Invalid amount.\n";
    }
    Utils::pause();
}

void Menu::customerWithdraw(Account* acc) {
    Utils::clearScreen();
    Utils::printBanner("WITHDRAWAL");
    std::cout << "Current Balance: R" << std::fixed << std::setprecision(2) << acc->balance << "\n";
    std::cout << "Withdrawal Amount: R";
    std::string amtStr;
    std::getline(std::cin, amtStr);
    try {
        double amt = std::stod(amtStr);
        custMgr_.withdraw(acc, amt);
    } catch (...) {
        std::cout << "  [!] Invalid amount.\n";
    }
    Utils::pause();
}

void Menu::customerTransfer(Account* acc) {
    Utils::clearScreen();
    Utils::printBanner("FUND TRANSFER");
    std::cout << "Current Balance: R" << std::fixed << std::setprecision(2) << acc->balance << "\n";
    std::cout << "Destination Account Number: ";
    std::string dest;
    std::getline(std::cin, dest);
    dest = Utils::trim(dest);
    std::cout << "Transfer Amount: R";
    std::string amtStr;
    std::getline(std::cin, amtStr);
    try {
        double amt = std::stod(amtStr);
        custMgr_.transfer(acc, dest, amt);
    } catch (...) {
        std::cout << "  [!] Invalid amount.\n";
    }
    Utils::pause();
}

void Menu::customerStatement(Account* acc) {
    Utils::clearScreen();
    txnMgr_.printStatement(std::string(acc->accountNumber));
    Utils::pause();
}

void Menu::customerChangePIN(Account* acc) {
    Utils::clearScreen();
    Utils::printBanner("CHANGE PIN");
    std::cout << "Current PIN: ";
    std::string oldPIN;
    std::getline(std::cin, oldPIN);
    std::cout << "New PIN (5 digits): ";
    std::string newPIN;
    std::getline(std::cin, newPIN);
    std::cout << "Confirm New PIN: ";
    std::string confirmPIN;
    std::getline(std::cin, confirmPIN);

    if (newPIN != confirmPIN) {
        std::cout << "  [!] PINs do not match.\n";
    } else {
        custMgr_.changePIN(acc, Utils::trim(oldPIN), Utils::trim(newPIN));
    }
    Utils::pause();
}

// ─── Branch Menu ──────────────────────────────────────────────────────────────
void Menu::branchMenu() {
    while (true) {
        Utils::clearScreen();
        Utils::printBanner("BRANCH INFORMATION");
        std::cout << "  1. View All Branches\n";
        std::cout << "  2. View Branch Details\n";
        std::cout << "  3. Inter-Branch Comparison\n";
        std::cout << "  0. Back\n";
        Utils::printDivider('-');
        std::cout << "Select: ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            branchMgr_.viewAllBranches();
            Utils::pause();
        } else if (choice == "2") {
            std::cout << "Branch Code: ";
            std::string code;
            std::getline(std::cin, code);
            branchMgr_.viewBranchDetails(Utils::toUpper(Utils::trim(code)));
            Utils::pause();
        } else if (choice == "3") {
            branchMgr_.interBranchComparison();
            Utils::pause();
        } else if (choice == "0") {
            break;
        }
    }
}

// ─── Report Menu ──────────────────────────────────────────────────────────────
void Menu::reportMenu(const std::string& branchCode) {
    while (true) {
        Utils::clearScreen();
        Utils::printBanner("REPORTS & ANALYTICS");
        std::cout << "  Branch: " << branchCode << "\n";
        Utils::printDivider('-');
        std::cout << "  1. Daily Transaction Report\n";
        std::cout << "  2. Customer Account Summary\n";
        std::cout << "  3. Branch Performance Report\n";
        std::cout << "  4. Export Transactions to CSV\n";
        std::cout << "  5. Export Customers to CSV\n";
        std::cout << "  6. Backup Data\n";
        std::cout << "  0. Back\n";
        Utils::printDivider('-');
        std::cout << "Select: ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            txnMgr_.printDailyReport();
            Utils::pause();
        } else if (choice == "2") {
            custMgr_.printAccountSummaryReport(branchCode);
            Utils::pause();
        } else if (choice == "3") {
            custMgr_.printBranchPerformanceReport();
            Utils::pause();
        } else if (choice == "4") {
            std::string fname = "exports/transactions_export.csv";
            if (txnMgr_.exportToCSV(fname))
                std::cout << "  Exported to " << fname << "\n";
            else
                std::cout << "  [!] Export failed. Check exports/ directory exists.\n";
            Utils::pause();
        } else if (choice == "5") {
            std::string fname = "exports/customers_export.csv";
            if (custMgr_.exportCustomersToCSV(fname))
                std::cout << "  Exported to " << fname << "\n";
            else
                std::cout << "  [!] Export failed. Check exports/ directory exists.\n";
            Utils::pause();
        } else if (choice == "6") {
            custMgr_.backupData();
            Utils::pause();
        } else if (choice == "0") {
            break;
        }
    }
}
