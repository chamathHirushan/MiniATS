#pragma once
#include "OrderBookEntry.hpp"
#include <vector>
#include <string>
#include <mutex>

class OrderBook {
    public:
        /** construct reading a CSV file */
        OrderBook(const std::string& filename);
        /** return a vector of all known products */
        std::vector<std::string> getKnownProducts();
        /** return all orders of specified type, product, and timestamp */
        std::vector<OrderBookEntry*> getOrders(OrderBookType type, const std::string& product);

        static double getHighPrice(const std::vector<OrderBookEntry>& orders);
        static double getLowPrice(const std::vector<OrderBookEntry>& orders);
        /** returns the average price for a product at a given timestamp */
        static double getAvgPrice(const std::vector<OrderBookEntry>& orders);
        static double getTotalVolume(const std::vector<OrderBookEntry>& orders);

        /** returns the earliest timestamp */
        std::string getEarliestTimestamp();
        /** returns the next timestamp after the input */
        std::string getNextTimestamp(const std::string& timestamp);
        std::string getFilename() const;

        /** insert a new order into the order book */
        void insertOrder(const OrderBookEntry& order);
        std::vector<OrderBookEntry> getOrders();
        /** insert new sales into the finalized sales book */
        void insertSales(std::vector<OrderBookEntry>& sales);
        std::vector<OrderBookEntry> getSales();
        
        /** match asks to bids and return a list of sales */
        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string currentTimestamp);

    private:
        std::string filename;
        std::vector<OrderBookEntry> finalizedSales;
        std::vector<OrderBookEntry> orders;
        mutable std::recursive_mutex ordersMutex; // Mutex lock to protect the orders,finalizedSales when multiple threads access
};