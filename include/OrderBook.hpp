#pragma once
#include "OrderBookEntry.hpp"
#include <vector>
#include <string>

class OrderBook {
    public:
        /** construct reading a CSV file */
        OrderBook(const std::string& filename);
        /** return a vector of all known products */
        std::vector<std::string> getKnownProducts();
        /** return all orders of specified type, product, and timestamp */
        std::vector<OrderBookEntry> getOrders(OrderBookType type, const std::string& product, const std::string& timestamp);

        static double getHighPrice(const std::vector<OrderBookEntry>& orders);
        static double getLowPrice(const std::vector<OrderBookEntry>& orders);

        std::string getEarliestTimestamp();
    private:
        std::vector<OrderBookEntry> orders;
};