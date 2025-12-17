#pragma once
#include <string>
#include <nlohmann/json.hpp>

enum class OrderBookType {bid, ask, sale};

class OrderBookEntry {
    public:
        double price;
        double amount;
        std::string timestamp;
        std::string product;
        OrderBookType orderType;
        std::string username;
        std::size_t id;
        
        OrderBookEntry() = default;
        OrderBookEntry(double price, 
                       double amount, 
                       std::string timestamp, 
                       std::string product, 
                       OrderBookType orderType,
                       std::string username = "default");

        static OrderBookType determineOrderType(const std::string& typeStr);
        static std::string orderTypeToString(const OrderBookType& type);
        static bool compareByTimestamp(const OrderBookEntry& a, const OrderBookEntry& b);
        static bool compareByPriceAsc(OrderBookEntry* a, OrderBookEntry* b);
        static bool compareByPriceDesc(OrderBookEntry* a, OrderBookEntry* b);

        friend void to_json(nlohmann::json& j, const OrderBookEntry& e);
        friend void from_json(const nlohmann::json& j, OrderBookEntry& e);
    
    private:
        static std::size_t nextId;
};

void to_json(nlohmann::json& j, const OrderBookEntry& e);
void from_json(const nlohmann::json& j, OrderBookEntry& e);