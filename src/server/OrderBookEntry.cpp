#include "OrderBookEntry.hpp"
#include <stdexcept>

std::size_t OrderBookEntry::nextId = 1;

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

            id = nextId++;
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

std::string OrderBookEntry::orderTypeToString(const OrderBookType& type) {
    switch(type) {
        case OrderBookType::bid: return "bid";
        case OrderBookType::ask: return "ask";
        case OrderBookType::sale: return "sale";
        default: return "unknown";
    }
}

bool OrderBookEntry::compareByTimestamp(const OrderBookEntry& a, const OrderBookEntry& b) {
    return a.timestamp < b.timestamp;
}

bool OrderBookEntry::compareByPriceAsc(OrderBookEntry* a, OrderBookEntry* b) {
    return a->price < b->price;
}

bool OrderBookEntry::compareByPriceDesc(OrderBookEntry* a, OrderBookEntry* b) {
    return a->price > b->price;
}