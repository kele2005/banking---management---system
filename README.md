#include "teller.h"
#include "globals.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <cstring>

// ─── Teller Constructor ───────────────────────────────────────────────────────
Teller::Teller() {
    std::memset(tellerID,          0, sizeof(tellerID));
    std::memset(fullName,          0, sizeof(fullName));
    std::memset(encryptedPassword, 0, sizeof(encryptedPassword));
    std::memset(branchCode,        0, sizeof(branchCode));
    isActive = true;
}

Teller::Teller(const std::string& id, const std::string& name,
               const std::string& plainPassword, const std::string& branch) {
    std::memset(tellerID,          0, sizeof(tellerID));
    std::memset(fullName,          0, sizeof(fullName));
    std::memset(encryptedPassword, 0, sizeof(encryptedPassword));
    std::memset(branchCode,        0, sizeof(branchCode));

    std::strncpy(tellerID,   id.c_str(),     sizeof(tellerID)-1);
    std::strncpy(fullName,   name.c_str(),   sizeof(fullName)-1);
    std::strncpy(branchCode, branch.c_str(), sizeof(branchCode)-1);
    isActive = true;

    // Encrypt and store password
    std::string enc = Utils::encrypt(plainPassword);
    std::strncpy(encryptedPassword, enc.c_str(), sizeof(encryptedPassword)-1);
}

bool Teller::verifyPassword(const std::string& plainPassword) const {
    std::string enc = Utils::encrypt(plainPassword);
    return enc == std::string(encryptedPassword);
}

// ─── TellerManager ────────────────────────────────────────────────────────────
TellerManager::TellerManager() {}

bool TellerManager::loadTellers() {
    std::ifstream file(FILE_TELLERS, std::ios::binary);
    if (!file.is_open()) return false;

    tellers_.clear();
    Teller t;
    while (file.read(reinterpret_cast<char*>(&t), sizeof(Teller))) {
        tellers_.push_back(t);
    }
    file.close();
    return !tellers_.empty();
}

bool TellerManager::saveTellers() {
    std::ofstream file(FILE_TELLERS, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) return false;

    for (const auto& t : tellers_) {
        file.write(reinterpret_cast<const char*>(&t), sizeof(Teller));
    }
    file.close();
    return true;
}

void TellerManager::initDefaultTellers() {
    if (!tellers_.empty()) return;

    // Default tellers for each branch
    tellers_.emplace_back("T001", "Fortunate Sibiya",  "teller123", "JHB");
    tellers_.emplace_back("T002", "Morwakgadi Mmamabolo",   "teller456", "CPT");
    tellers_.emplace_back("T003", "Nkateko Nkuna",    "teller789", "DBN");
    tellers_.emplace_back("T004", "Kelebogile Rangata", "admin001",  "JHB");

    saveTellers();
}

Teller* TellerManager::authenticate(const std::string& id, const std::string& password) {
    for (auto& t : tellers_) {
        if (std::string(t.tellerID) == id && t.isActive) {
            if (t.verifyPassword(password)) {
                return &t;
            }
        }
    }
    return nullptr;
}

bool TellerManager::tellerExists(const std::string& id) const {
    for (const auto& t : tellers_) {
        if (std::string(t.tellerID) == id) return true;
    }
    return false;
}

bool TellerManager::addTeller(const Teller& t) {
    if (tellerExists(t.getID())) return false;
    tellers_.push_back(t);
    return saveTellers();
}
