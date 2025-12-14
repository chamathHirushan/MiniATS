#include "Wallet.hpp"
#include <stdexcept>
#include "CSVHandler.hpp"

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

bool Wallet::canFulfillOrder(OrderBookEntry& order) {
    std::vector<std::string> tokens = CSVHandler::extractTokens(order.product, '/');
    if (tokens.size() != 2) {
        throw std::invalid_argument("Invalid product format in order");
    }
    
    if (order.orderType == OrderBookType::ask) {
        return containsAmmount(tokens[0], order.amount); // to sell we only need the selling currency quantity
    } else if (order.orderType == OrderBookType::bid) {
        double requiredQuoteAmount = order.amount * order.price; // to buy we need the selling currency quantity, times the price per unit
        return containsAmmount(tokens[1], requiredQuoteAmount);
    }
    return false;
}

void Wallet::processSale(OrderBookEntry& sale) {
    std::vector<std::string> tokens = CSVHandler::extractTokens(sale.product, '/');
    if (tokens.size() != 2) {
        throw std::invalid_argument("Invalid product format in sale");
    }
    
    if (sale.orderType == OrderBookType::asksale) {
        // Seller side
        removeCurrency(tokens[0], sale.amount); // remove the sold currency
        insertCurrency(tokens[1], sale.amount * sale.price); // add the received currency
    } else if (sale.orderType == OrderBookType::bidsale) {
        // Buyer side
        double totalCost = sale.amount * sale.price;
        removeCurrency(tokens[1], totalCost); // remove the spent currency
        insertCurrency(tokens[0], sale.amount); // add the purchased currency
    }
}