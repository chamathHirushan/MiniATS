#pragma once
#include "OrderBookEntry.hpp"
#include <vector>
#include <string>
#include <mutex>
#include "User.hpp"
#include "UserStore.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <unordered_map>

class OrderBook {
    public:
        /** construct reading a CSV file */
        OrderBook(const std::string& filename);
        /** return a vector of all known products */
        std::vector<std::string> getKnownProducts();
        /** return all orders of specified type, product, and timestamp */
        std::unordered_map<OrderBookType, std::vector<OrderBookEntry*>> getOrders(const std::string& product);

        static double getHighPrice(const std::vector<OrderBookEntry*>& orders);
        static double getLowPrice(const std::vector<OrderBookEntry*>& orders);
        /** returns the average price for a product at a given timestamp */
        static double getAvgPrice(const std::vector<OrderBookEntry*>& orders);
        static double getTotalVolume(const std::vector<OrderBookEntry*>& orders);

        /** returns the earliest timestamp */
        std::string getEarliestTimestamp();
        /** returns the next timestamp after the input */
        std::string getNextTimestamp(const std::string& timestamp);
        std::string getFilename() const;

        /** insert a new order into the order book */
        void insertOrder(const OrderBookEntry& order);
        std::vector<OrderBookEntry> getAllOrders();
        std::vector<OrderBookEntry> getOrdersForUser(const std::string& username);
        /** insert new sales into the finalized sales book */
        void insertSales(std::vector<OrderBookEntry>& sales);
        std::vector<OrderBookEntry> getSales();
        /** remove an order by its unique ID */
        bool removeOrderById(std::size_t id);
        
        /** match asks to bids and return a list of sales */
        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string currentTimestamp, UserStore& userStore);

        void save();
        void load();

    private:
        std::string filename;
        std::vector<OrderBookEntry> finalizedSales;
        //std::vector<OrderBookEntry> orders;
        std::unordered_map<std::string, std::vector<OrderBookEntry>> orderMap;

        mutable std::recursive_mutex ordersMutex; // Mutex lock to protect the orders,finalizedSales when multiple threads access

        /** Implementation to update buyer and seller wallets based on the sale */
        void processSale(User& buyer, User& seller, const OrderBookEntry& sale, double bid_price, double ask_price);
        /** Remove matched orders from the order book */
        void removeMatchedOrders(const std::string& product);
};