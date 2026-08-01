#ifndef TELLER_H
#define TELLER_H

#include <string>
#include <vector>

// ─── Teller Class ─────────────────────────────────────────────────────────────
class Teller {
public:
    char tellerID[10];          // e.g. T001
    char fullName[60];
    char encryptedPassword[64]; // XOR-encrypted
    char branchCode[10];
    bool isActive;

    Teller();
    Teller(const std::string& id, const std::string& name,
           const std::string& plainPassword, const std::string& branch);

    bool verifyPassword(const std::string& plainPassword) const;
    std::string getID()     const { return std::string(tellerID); }
    std::string getName()   const { return std::string(fullName); }
    std::string getBranch() const { return std::string(branchCode); }
};

// ─── Teller Manager ───────────────────────────────────────────────────────────
class TellerManager {
public:
    TellerManager();

    bool loadTellers();
    bool saveTellers();

    void initDefaultTellers();   // seed default tellers if file absent

    // Returns pointer to authenticated teller, nullptr on failure
    Teller* authenticate(const std::string& id, const std::string& password);

    bool tellerExists(const std::string& id) const;
    bool addTeller(const Teller& t);

    const std::vector<Teller>& getTellers() const { return tellers_; }

private:
    std::vector<Teller> tellers_;
};

#endif // TELLER_H
