#pragma once
#include <string>

enum class OrderBookType {bid, ask, unknown};

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
};