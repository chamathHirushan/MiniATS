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
    std::vector<OrderBookEntry> filteredEntries;
    // Logic to populate filteredEntries vector
    return filteredEntries;
}