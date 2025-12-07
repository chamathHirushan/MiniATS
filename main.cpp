#include "OrderBookEntry.hpp"
#include "MerkelMain.hpp"
#include "Extractor.hpp"

int main() {
    // MerkelMain merkelApp;
    // merkelApp.init();
    test();
    return 0;
} 

// double computeAveragePrice(std::vector<OrderBookEntry>& entries) {
//     double sum = 0;
//     for (const OrderBookEntry& e : entries) {
//         sum += e.price;
//     }
//     return sum / entries.size();
// }

// double computeLowPrice(std::vector<OrderBookEntry>& entries) {
//     double min = entries[0].price;
//     for (const OrderBookEntry& e : entries) {
//         if (e.price < min) {
//             min = e.price;
//         }
//     }
//     return min;
// }

// double computeHighPrice(std::vector<OrderBookEntry>& entries) {
//     double max = entries[0].price;
//     for (const OrderBookEntry& e : entries) {
//         if (e.price > max) {
//             max = e.price;
//         }
//     }
//     return max;
// }

// double computePriceSpread(std::vector<OrderBookEntry>& entries) {
//     return computeHighPrice(entries) - computeLowPrice(entries);
// }

// int main() {
//     std::vector<OrderBookEntry> entries;

//     entries.push_back(OrderBookEntry(1000.0, 
//                                      0.02, 
//                                      "2020/03/17 17:01:24.884492", 
//                                      "BTC/USDT", 
//                                      OrderBookType::bid));

//     entries.push_back(OrderBookEntry(2000.0, 
//                                      0.02, 
//                                      "2020/03/17 17:01:24.884492", 
//                                      "BTC/USDT", 
//                                      OrderBookType::ask));

//     for (OrderBookEntry& e : entries) {
//         std::cout << "Price: " << e.price << std::endl;
//     }

//     std::cout << "Average Price: " << computeAveragePrice(entries) << std::endl;
//     std::cout << "Low Price: " << computeLowPrice(entries) << std::endl;
//     std::cout << "High Price: " << computeHighPrice(entries) << std::endl;
//     std::cout << "Price Spread: " << computePriceSpread(entries) << std::endl;

//     while (true) {
//         printMenu();
//         int userOption = getUserOption();
//         processUserOption(userOption);
//     }
//     return 0;
// }
