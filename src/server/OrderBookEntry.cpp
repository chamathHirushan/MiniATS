#include "OrderBookEntry.hpp"
#include <stdexcept>

OrderBookEntry:: OrderBookEntry(double price, 
                       double amount, 
                       std::string timestamp, 
                       std::string product, 
                       OrderBookType orderType,
                       std::string username)
        {
            this->price = price;
            this->amount = amount;   
            this->timestamp = timestamp;
            this->product = product;
            this->orderType = orderType;
            this->username = username;
        }

OrderBookType OrderBookEntry::determineOrderType(const std::string& typeStr) {
    if (typeStr == "bid") {
        return OrderBookType::bid;
    } else if (typeStr == "ask") {
        return OrderBookType::ask;
    } else {
        throw std::invalid_argument("Invalid order type string: " + typeStr);
    }
}

bool OrderBookEntry::compareByTimestamp(const OrderBookEntry& a, const OrderBookEntry& b) {
    return a.timestamp < b.timestamp;
}

bool OrderBookEntry::compareByPriceAsc(const OrderBookEntry& a, const OrderBookEntry& b) {
    return a.price < b.price;
}

bool OrderBookEntry::compareByPriceDesc(const OrderBookEntry& a, const OrderBookEntry& b) {
    return a.price > b.price;
}