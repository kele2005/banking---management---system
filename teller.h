#ifndef BRANCH_H
#define BRANCH_H

#include <string>
#include <vector>
#include "globals.h"

// ─── Branch Struct ────────────────────────────────────────────────────────────
struct Branch {
    char code[10];          // e.g. "JHB", "CPT", "DBN"
    char name[50];          // e.g. "Johannesburg Main"
    char address[100];
    char phone[15];
    char manager[60];
    int  totalAccounts;
    double totalDeposits;
    double totalWithdrawals;

    Branch();
};

// ─── Branch Manager ───────────────────────────────────────────────────────────
class BranchManager {
public:
    BranchManager();

    bool loadBranches();
    bool saveBranches();

    void initDefaultBranches();   // seed 3 branches if file absent

    void viewAllBranches() const;
    void viewBranchDetails(const std::string& code) const;
    void interBranchComparison() const;

    bool branchExists(const std::string& code) const;
    Branch* getBranch(const std::string& code);

    void updateStats(const std::string& branchCode, double depositDelta, double withdrawalDelta, int accountDelta);

    const std::vector<Branch>& getBranches() const { return branches_; }

private:
    std::vector<Branch> branches_;
};

#endif // BRANCH_H
