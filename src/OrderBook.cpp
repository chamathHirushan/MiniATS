#include "OrderBook.hpp"
#include <vector>
#include <string>
#include "CSVReader.hpp"
#include <map>

OrderBook::OrderBook(const std::string& filename) {
    // Load entries from the CSV file
    orders = CSVReader::readCSV(filename);
}

std::vector<std::string> OrderBook::getKnownProducts() {
    // Implementation to extract known products from entries
    std::vector<std::string> products;
    std::map<std::string, bool> productMap;
    for (const OrderBookEntry& entry : orders) {
        productMap[entry.product] = true;
    }
    for (const auto& pair : productMap) {
        products.push_back(pair.first);
    }
    return products;
}

std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, const std::string& product, const std::string& timestamp) {
    // Implementation to filter entries based on type, product, and timestamp
    std::vector<OrderBookEntry> filteredOrders;
    for (const OrderBookEntry& entry : orders) {
        if (entry.orderType == type && entry.product == product && entry.timestamp == timestamp) {
            filteredOrders.push_back(entry);
        }
    }

    return filteredOrders;
}

double OrderBook::getHighPrice(const std::vector<OrderBookEntry>& orders) {
    if (orders.empty()) return 0.0;
    double max = orders[0].price;
    for (const OrderBookEntry& e : orders) {
        if (e.price > max) {
            max = e.price;
        }
    }
    return max;
}

double OrderBook::getLowPrice(const std::vector<OrderBookEntry>& orders) {
    if (orders.empty()) return 0.0;
    double min = orders[0].price;
    for (const OrderBookEntry& e : orders) {
        if (e.price < min) {
            min = e.price;
        }
    }
    return min;
}

double OrderBook::getAvgPrice(const std::vector<OrderBookEntry>& orders) {
    double total = 0.0;
    int count = 0;
    double totalVolume = 0.0;
    for (const OrderBookEntry& entry : orders) {
        total += entry.price * entry.amount;
        totalVolume += entry.amount;
    }
    if (totalVolume == 0) return 0.0;
    return total / totalVolume;
}

double OrderBook::getTotalVolume(const std::vector<OrderBookEntry>& orders) {
    double totalVolume = 0.0;
    for (const OrderBookEntry& entry : orders) {
            totalVolume += entry.amount;
        }
    return totalVolume;
}

std::string OrderBook::getEarliestTimestamp() {
    if (orders.empty()) {
        return "";
    }
    std::string earliest = orders[0].timestamp;
    
    for (const OrderBookEntry& e : orders) {
        if (e.timestamp < earliest) {
            earliest = e.timestamp;
        }
    }
    return earliest;
}

std::string OrderBook::getNextTimestamp(const std::string& timestamp) {
    std::string nextTimestamp = "";
    for (const OrderBookEntry& e : orders) {
        if (e.timestamp > timestamp) {
            if (nextTimestamp == "" || e.timestamp < nextTimestamp) {
                nextTimestamp = e.timestamp;
            }
        }
    }
    return nextTimestamp;
}