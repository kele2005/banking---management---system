/*
 * ============================================================
 *  Multi-Branch Banking Management System
 *  Standard Bank – Console Prototype
 *
 *  PROGRAMMING 621 Assignment
 *  Richfield Graduate Institute of Technology
 * ============================================================
 *
 *  Compile (g++):
 *    g++ -std=c++17 -Wall -o banking src/main.cpp src/utils.cpp \
 *        src/branch.cpp src/teller.cpp src/account.cpp \
 *        src/transaction.cpp src/customer_manager.cpp src/menu.cpp \
 *        -I include
 *
 *  Default Teller Credentials:
 *    T001 / teller123  (Johannesburg)
 *    T002 / teller456  (Cape Town)
 *    T003 / teller789  (Durban)
 *    T004 / admin001   (Johannesburg – senior)
 * ============================================================
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>

// Create a directory if it doesn't exist (portable helper)
#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(p) _mkdir(p)
#else
  #include <sys/stat.h>
  #define MKDIR(p) mkdir(p, 0755)
#endif

#include "globals.h"
#include "utils.h"
#include "branch.h"
#include "teller.h"
#include "account.h"
#include "transaction.h"
#include "customer_manager.h"
#include "menu.h"

// ─── Ensure required directories exist ───────────────────────────────────────
static void ensureDirectories() {
    MKDIR("data");
    MKDIR("data/backup");
    MKDIR("exports");
}

// ─── Write default system_config.txt if absent ───────────────────────────────
static void ensureSystemConfig() {
    std::ifstream check(FILE_CONFIG);
    if (check.is_open()) { check.close(); return; }

    std::ofstream cfg(FILE_CONFIG);
    cfg << "# Standard Bank – Multi-Branch Banking System Config\n";
    cfg << "system_name=Standard Bank Console System\n";
    cfg << "version=1.0.0\n";
    cfg << "max_login_attempts=3\n";
    cfg << "pin_length=5\n";
    cfg << "interest_frequency=monthly\n";
    cfg.close();
}

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {
    // Seed random number generator for PIN generation
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Prepare filesystem
    ensureDirectories();
    ensureSystemConfig();

    // ── Initialise managers ──────────────────────────────────────────────────
    BranchManager      branchMgr;
    TellerManager      tellerMgr;
    TransactionManager txnMgr;
    CustomerManager    custMgr(txnMgr, branchMgr);

    // ── Load persisted data (or seed defaults) ───────────────────────────────
    if (!branchMgr.loadBranches()) {
        branchMgr.initDefaultBranches();
    }

    if (!tellerMgr.loadTellers()) {
        tellerMgr.initDefaultTellers();
    }

    txnMgr.loadTransactions();   // OK if file absent (first run)
    custMgr.loadCustomers();     // OK if file absent (first run)

    // ── Launch UI ────────────────────────────────────────────────────────────
    Menu menu(tellerMgr, custMgr, txnMgr, branchMgr);
    menu.run();

    return 0;
}
