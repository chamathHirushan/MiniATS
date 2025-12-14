#pragma once
#include <string>

enum class OrderBookType {bid, ask, asksale, bidsale};

class OrderBookEntry {
    public:
        double price;
        double amount;
        std::string timestamp;
        std::string product;
        OrderBookType orderType;
        std::string username;
        
        OrderBookEntry(double price, 
                       double amount, 
                       std::string timestamp, 
                       std::string product, 
                       OrderBookType orderType,
                       std::string username = "dataset");

        static OrderBookType determineOrderType(const std::string& typeStr);
        static bool compareByTimestamp(const OrderBookEntry& a, const OrderBookEntry& b);
        static bool compareByPriceAsc(OrderBookEntry* a, OrderBookEntry* b);
        static bool compareByPriceDesc(OrderBookEntry* a, OrderBookEntry* b);
};