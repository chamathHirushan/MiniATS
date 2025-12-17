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
    } else if (typeStr == "sale") {
        return OrderBookType::sale;
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

void to_json(nlohmann::json& j, const OrderBookEntry& e) {
    j = nlohmann::json{
        {"price", e.price},
        {"amount", e.amount},
        {"timestamp", e.timestamp},
        {"product", e.product},
        {"orderType", OrderBookEntry::orderTypeToString(e.orderType)},
        {"username", e.username},
        {"id", e.id}
    };
}

void from_json(const nlohmann::json& j, OrderBookEntry& e) {
    j.at("price").get_to(e.price);
    j.at("amount").get_to(e.amount);
    j.at("timestamp").get_to(e.timestamp);
    j.at("product").get_to(e.product);
    
    std::string typeStr;
    j.at("orderType").get_to(typeStr);
    e.orderType = OrderBookEntry::determineOrderType(typeStr);
    
    j.at("username").get_to(e.username);
    if (j.contains("id")) {
        j.at("id").get_to(e.id);
        // Ensure global nextId is consistent if we strictly relied on it, 
        // but here we just load the ID. 
        // OrderBook::load will handle checking max ID if we implement it there.
    }
}