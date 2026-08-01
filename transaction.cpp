#include "branch.h"
#include "globals.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <string>

// ─── Branch Constructor ───────────────────────────────────────────────────────
Branch::Branch() {
    std::memset(code,     0, sizeof(code));
    std::memset(name,     0, sizeof(name));
    std::memset(address,  0, sizeof(address));
    std::memset(phone,    0, sizeof(phone));
    std::memset(manager,  0, sizeof(manager));
    totalAccounts    = 0;
    totalDeposits    = 0.0;
    totalWithdrawals = 0.0;
}

// ─── BranchManager ────────────────────────────────────────────────────────────
BranchManager::BranchManager() {}

bool BranchManager::loadBranches() {
    std::string path = FILE_BRANCHES;
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return false;

    branches_.clear();
    Branch b;
    while (std::fread(&b, sizeof(Branch), 1, file) == 1) {
        branches_.push_back(b);
    }
    std::fclose(file);
    return !branches_.empty();
}

bool BranchManager::saveBranches() {
    std::string path = FILE_BRANCHES;
    std::FILE* file = std::fopen(path.c_str(), "wb");
    if (!file) return false;

    for (const auto& b : branches_) {
        if (std::fwrite(&b, sizeof(Branch), 1, file) != 1) {
            std::fclose(file);
            return false;
        }
    }
    std::fclose(file);
    return true;
}

void BranchManager::initDefaultBranches() {
    if (!branches_.empty()) return;

    // Branch 1 – Johannesburg
    Branch b1;
    std::strncpy(b1.code,    "JHB",                      sizeof(b1.code)-1);
    std::strncpy(b1.name,    "Johannesburg Main",         sizeof(b1.name)-1);
    std::strncpy(b1.address, "1 Simmonds St, Johannesburg", sizeof(b1.address)-1);
    std::strncpy(b1.phone,   "0117001000",                sizeof(b1.phone)-1);
    std::strncpy(b1.manager, "Thabo Nkosi",               sizeof(b1.manager)-1);
    branches_.push_back(b1);

    // Branch 2 – Cape Town
    Branch b2;
    std::strncpy(b2.code,    "CPT",                       sizeof(b2.code)-1);
    std::strncpy(b2.name,    "Cape Town Central",         sizeof(b2.name)-1);
    std::strncpy(b2.address, "5 Adderley St, Cape Town",  sizeof(b2.address)-1);
    std::strncpy(b2.phone,   "0217002000",                sizeof(b2.phone)-1);
    std::strncpy(b2.manager, "Ayesha Davids",             sizeof(b2.manager)-1);
    branches_.push_back(b2);

    // Branch 3 – Durban
    Branch b3;
    std::strncpy(b3.code,    "DBN",                       sizeof(b3.code)-1);
    std::strncpy(b3.name,    "Durban Harbour",            sizeof(b3.name)-1);
    std::strncpy(b3.address, "10 Victoria Embankment, Durban", sizeof(b3.address)-1);
    std::strncpy(b3.phone,   "0317003000",                sizeof(b3.phone)-1);
    std::strncpy(b3.manager, "Priya Naidoo",              sizeof(b3.manager)-1);
    branches_.push_back(b3);

    saveBranches();
}

void BranchManager::viewAllBranches() const {
    Utils::printBanner("ALL BRANCHES");
    std::cout << std::left
              << std::setw(8)  << "Code"
              << std::setw(25) << "Name"
              << std::setw(20) << "Manager"
              << std::setw(12) << "Accounts"
              << "\n";
    Utils::printDivider('-');
    for (const auto& b : branches_) {
        std::cout << std::left
                  << std::setw(8)  << b.code
                  << std::setw(25) << b.name
                  << std::setw(20) << b.manager
                  << std::setw(12) << b.totalAccounts
                  << "\n";
    }
    Utils::printDivider('-');
}

void BranchManager::viewBranchDetails(const std::string& code) const {
    for (const auto& b : branches_) {
        if (std::string(b.code) == code) {
            Utils::printBanner("BRANCH DETAILS");
            std::cout << "Code     : " << b.code    << "\n";
            std::cout << "Name     : " << b.name    << "\n";
            std::cout << "Address  : " << b.address << "\n";
            std::cout << "Phone    : " << b.phone   << "\n";
            std::cout << "Manager  : " << b.manager << "\n";
            std::cout << "Accounts : " << b.totalAccounts << "\n";
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Deposits : R" << b.totalDeposits    << "\n";
            std::cout << "Withdrawals: R" << b.totalWithdrawals << "\n";
            return;
        }
    }
    std::cout << "Branch not found.\n";
}

void BranchManager::interBranchComparison() const {
    Utils::printBanner("INTER-BRANCH COMPARISON");
    std::cout << std::left
              << std::setw(8)  << "Code"
              << std::setw(25) << "Name"
              << std::setw(12) << "Accounts"
              << std::setw(18) << "Total Deposits"
              << std::setw(18) << "Total Withdrawals"
              << "\n";
    Utils::printDivider('-');
    for (const auto& b : branches_) {
        std::cout << std::left
                  << std::setw(8)  << b.code
                  << std::setw(25) << b.name
                  << std::setw(12) << b.totalAccounts
                  << std::setw(18) << std::fixed << std::setprecision(2) << b.totalDeposits
                  << std::setw(18) << b.totalWithdrawals
                  << "\n";
    }
    Utils::printDivider('-');
}

bool BranchManager::branchExists(const std::string& code) const {
    for (const auto& b : branches_) {
        if (std::string(b.code) == code) return true;
    }
    return false;
}

Branch* BranchManager::getBranch(const std::string& code) {
    for (auto& b : branches_) {
        if (std::string(b.code) == code) return &b;
    }
    return nullptr;
}

void BranchManager::updateStats(const std::string& branchCode,
                                double depositDelta,
                                double withdrawalDelta,
                                int accountDelta) {
    Branch* b = getBranch(branchCode);
    if (b) {
        b->totalDeposits    += depositDelta;
        b->totalWithdrawals += withdrawalDelta;
        b->totalAccounts    += accountDelta;
        saveBranches();
    }
}
