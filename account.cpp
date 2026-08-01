#ifndef MENU_H
#define MENU_H

#include "teller.h"
#include "customer_manager.h"
#include "transaction.h"
#include "branch.h"

// ─── Menu / UI Controller ─────────────────────────────────────────────────────
class Menu {
public:
    Menu(TellerManager& tellerMgr,
         CustomerManager& custMgr,
         TransactionManager& txnMgr,
         BranchManager& branchMgr);

    void run();   // main entry point

private:
    TellerManager&    tellerMgr_;
    CustomerManager&  custMgr_;
    TransactionManager& txnMgr_;
    BranchManager&    branchMgr_;

    // ── Top-level menus ───────────────────────────────────────────────────────
    void mainMenu();
    void tellerLoginFlow();
    void customerLoginFlow();

    // ── Teller menus ─────────────────────────────────────────────────────────
    void tellerMenu(Teller* teller);
    void tellerRegisterCustomer(Teller* teller);
    void tellerViewCustomer(Teller* teller);
    void tellerProcessTransaction(Teller* teller);
    void tellerBranchReports(Teller* teller);
    void tellerApplyInterest(Teller* teller);
    void tellerSearchCustomer(Teller* teller);

    // ── Customer menus ────────────────────────────────────────────────────────
    void customerMenu(Account* acc);
    void customerDeposit(Account* acc);
    void customerWithdraw(Account* acc);
    void customerTransfer(Account* acc);
    void customerStatement(Account* acc);
    void customerChangePIN(Account* acc);

    // ── Branch menus ──────────────────────────────────────────────────────────
    void branchMenu();

    // ── Report menus ─────────────────────────────────────────────────────────
    void reportMenu(const std::string& branchCode);
};

#endif // MENU_H
