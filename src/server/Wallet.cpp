#include "Wallet.hpp"
#include <stdexcept>
#include "CSVHandler.hpp"
#include <nlohmann/json.hpp>

Wallet::Wallet(const Wallet& other) {
    std::lock_guard<std::recursive_mutex> lock(other.mtx);
    currencies = other.currencies;
    locked = other.locked;
}

Wallet& Wallet::operator=(const Wallet& other) {
    if (this != &other) {
        std::unique_lock<std::recursive_mutex> lockSelf(mtx, std::defer_lock);
        std::unique_lock<std::recursive_mutex> lockOther(other.mtx, std::defer_lock);
        std::lock(lockSelf, lockOther);
        currencies = other.currencies;
        locked = other.locked;
    }
    return *this;
}

void Wallet::insertCurrency(std::string type, double amount) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (amount < 0) {
        throw std::invalid_argument("Amount cannot be negative");
    }
    currencies[type] += amount;
}

bool Wallet::removeCurrency(std::string type, double amount) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
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
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (amount < 0) {
        throw std::invalid_argument("Amount cannot be negative");
    }
    auto it = currencies.find(type);
    if (it != currencies.end()) {
        return it->second >= amount;
    }
    return 0.0 >= amount;
}

std::string Wallet::toString() {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::string result = "Wallet contains:\n";
    if (currencies.empty()) {
        result += "     (empty)\n";
        return result;
    }
    for (const auto& pair : currencies) {
        if (pair.second > 0)
        result += "     " + pair.first + ": " + std::to_string(pair.second) + "\n";
    }
    for (const auto& pair : locked) {
        if (pair.second > 0)
        result += "     (Locked) " + pair.first + ": " + std::to_string(pair.second) + "\n";
    }
    return result;
}

bool Wallet::fulfillOrder(OrderBookEntry& order) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::vector<std::string> tokens = CSVHandler::extractTokens(order.product, '/');
    if (tokens.size() != 2) {
        throw std::invalid_argument("Invalid product format in order");
    }
    
    if (order.orderType == OrderBookType::ask) {
        return lockCurrency(tokens[0], order.amount); // to sell we only need the selling currency quantity
    } else if (order.orderType == OrderBookType::bid) {
        double requiredQuoteAmount = order.amount * order.price; // to buy we need the selling currency quantity, times the price per unit
        return lockCurrency(tokens[1], requiredQuoteAmount);
    }
    return false;
}

bool Wallet::lockCurrency(const std::string& type, double amount) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (containsAmmount(type, amount)) {
        currencies[type] -= amount;
        locked[type] += amount;
        return true;
    }
    return false;
}

bool Wallet::cancelOrder(OrderBookEntry& order) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::vector<std::string> tokens = CSVHandler::extractTokens(order.product, '/');
    if (tokens.size() != 2) {
        throw std::invalid_argument("Invalid product format in order");
    }
    
    if (order.orderType == OrderBookType::ask) {
        return unlockCurrency(tokens[0], order.amount); // unlock the selling currency quantity
    } else if (order.orderType == OrderBookType::bid) {
        double requiredQuoteAmount = order.amount * order.price; // unlock the selling currency quantity, times the price per unit
        return unlockCurrency(tokens[1], requiredQuoteAmount);
    }
    return false;
}

bool Wallet::unlockCurrency(const std::string& type, double amount) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (locked[type] >= amount) {
        locked[type] -= amount;
        currencies[type] += amount;
        return true;
    }
    return false;
}

bool Wallet::spendLocked(const std::string& type, double amount) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (locked[type] >= amount) {
        locked[type] -= amount;
        return true;
    }
    return false;
}

void to_json(nlohmann::json& j, const Wallet& w) {
    std::lock_guard<std::recursive_mutex> lock(w.mtx);
    j = nlohmann::json{
        {"currencies", w.currencies},
        {"locked", w.locked}
    };
}

void from_json(const nlohmann::json& j, Wallet& w) {
    if (j.contains("currencies"))
        j.at("currencies").get_to(w.currencies);
    if (j.contains("locked"))
        j.at("locked").get_to(w.locked);
}