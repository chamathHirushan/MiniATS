#pragma once
#include <string>

enum class OrderBookType {bid, ask, sale};

class OrderBookEntry {
    public:
        double price;
        double amount;
        std::string timestamp;
        std::string product;
        OrderBookType orderType;
        
        OrderBookEntry(double price, 
                       double amount, 
                       std::string timestamp, 
                       std::string product, 
                       OrderBookType orderType);

        static OrderBookType determineOrderType(const std::string& typeStr);
        static bool compareByTimestamp(const OrderBookEntry& a, const OrderBookEntry& b);
        static bool compareByPriceAsc(const OrderBookEntry& a, const OrderBookEntry& b);
        static bool compareByPriceDesc(const OrderBookEntry& a, const OrderBookEntry& b);
};