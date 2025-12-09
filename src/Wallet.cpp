#include "Wallet.hpp"
#include <stdexcept>

void Wallet::insertCurrency(std::string type, double amount) {
    if (amount < 0) {
        throw std::invalid_argument("Amount cannot be negative");
    }
    currencies[type] += amount;
}

bool Wallet::removeCurrency(std::string type, double amount) {
    if (amount < 0) {
        throw std::invalid_argument("Amount cannot be negative");
    }
    if (containsAmmount(type, amount)) {
        currencies[type] -= amount;
        return true;
    }
    return false;
}

bool Wallet::containsAmmount(std::string type, double amount) {
    if (amount < 0) {
        throw std::invalid_argument("Amount cannot be negative");
    }
    return currencies[type] >= amount;
}

std::string Wallet::toString() {
    std::string result = "Wallet contents:\n";
    for (const auto& pair : currencies) {
        result += pair.first + ": " + std::to_string(pair.second) + "\n";
    }
    return result;
}