#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <ctime>

// ─── Utility / Helper Functions ──────────────────────────────────────────────

namespace Utils {

    // Simple XOR-based encryption for passwords and PINs
    std::string encrypt(const std::string& plaintext, unsigned char key = 0x5A);
    std::string decrypt(const std::string& ciphertext, unsigned char key = 0x5A);

    // Generate a random 5-digit PIN string
    std::string generatePIN();

    // Generate account number: ACC-<branchCode>-<5-digit seq>
    std::string generateAccountNumber(const std::string& branchCode, int sequence);

    // Validation helpers
    bool validateSAID(const std::string& id);
    bool validateEmail(const std::string& email);
    bool validatePhone(const std::string& phone);
    bool validateDate(const std::string& date);   // DD/MM/YYYY

    // Get current timestamp as formatted string
    std::string currentTimestamp();

    // Clear console screen (cross-platform)
    void clearScreen();

    // Pause and wait for Enter
    void pause();

    // Print a horizontal divider
    void printDivider(char ch = '=', int width = 60);

    // Print a centred title banner
    void printBanner(const std::string& title);

    // Convert AccountType enum to readable string
    std::string accountTypeStr(int type);

    // Convert TransactionType enum to readable string
    std::string transactionTypeStr(int type);

    // Trim whitespace from both ends
    std::string trim(const std::string& s);

    // Convert string to uppercase
    std::string toUpper(const std::string& s);
}

#endif // UTILS_H
