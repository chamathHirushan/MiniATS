#include "OrderBook.hpp"
#include <vector>
#include <string>
#include "CSVHandler.hpp"
#include <map>
#include <algorithm>
#include <iostream>
#include "UserStore.hpp"
#include "OrderBookEntry.hpp"
#include "User.hpp"

OrderBook::OrderBook(const std::string& filename) {
    // Load entries from the CSV file
    orderMap = CSVHandler::readCSV(filename);
    this->filename = filename;
}

std::vector<std::string> OrderBook::getKnownProducts() {
    std::lock_guard<std::recursive_mutex> lock(ordersMutex); 
    // Implementation to extract known products from entries
    std::vector<std::string> products;
    for (const auto& pair : orderMap) {
        products.push_back(pair.first);
    }
    return products;
}

std::unordered_map<OrderBookType, std::vector<OrderBookEntry*>> OrderBook::getOrders(const std::string& product) {
    std::lock_guard<std::recursive_mutex> lock(ordersMutex);
    // get product orders by types
    std::unordered_map<OrderBookType, std::vector<OrderBookEntry*>> filteredOrders;
    auto it = orderMap.find(product);
    if (it == orderMap.end()) {
        return filteredOrders;
    }
    for (OrderBookEntry& entry : it->second) {
        if (entry.orderType == OrderBookType::bid || entry.orderType == OrderBookType::ask) {
            filteredOrders[entry.orderType].push_back(&entry);
        }
    }

    return filteredOrders;
}

double OrderBook::getHighPrice(const std::vector<OrderBookEntry*>& orders) {
    if (orders.empty()) return 0.0;
    double max = orders[0]->price;
    for (const OrderBookEntry* e : orders) {
        if (e->price > max) {
            max = e->price;
        }
    }
    return max;
}

double OrderBook::getLowPrice(const std::vector<OrderBookEntry*>& orders) {
    if (orders.empty()) return 0.0;
    double min = orders[0]->price;
    for (const OrderBookEntry* e : orders) {
        if (e->price < min) {
            min = e->price;
        }
    }
    return min;
}

double OrderBook::getAvgPrice(const std::vector<OrderBookEntry*>& orders) {
    double total = 0.0;
    int count = 0;
    double totalVolume = 0.0;
    for (const OrderBookEntry* entry : orders) {
        total += entry->price * entry->amount;
        totalVolume += entry->amount;
    }
    if (totalVolume == 0.0) return 0.0;
    return total / totalVolume;
}

double OrderBook::getTotalVolume(const std::vector<OrderBookEntry*>& orders) {
    double totalVolume = 0.0;
    for (const OrderBookEntry* entry : orders) {
            totalVolume += entry->amount;
        }
    return totalVolume;
}

// std::string OrderBook::getEarliestTimestamp() {
//     std::lock_guard<std::recursive_mutex> lock(ordersMutex);
//     if (orders.empty()) {
//         return "";
//     }
//     std::string earliest = orders[0].timestamp;
    
//     for (const OrderBookEntry& e : orders) {
//         if (e.timestamp < earliest) {
//             earliest = e.timestamp;
//         }
//     }
//     return earliest;
// }

// std::string OrderBook::getNextTimestamp(const std::string& timestamp) {
//     std::lock_guard<std::recursive_mutex> lock(ordersMutex);
//     std::string nextTimestamp = "";
//     for (const OrderBookEntry& e : orders) {
//         if (e.timestamp > timestamp) {
//             if (nextTimestamp == "" || e.timestamp < nextTimestamp) {
//                 nextTimestamp = e.timestamp;
//             }
//         }
//     }
//     return nextTimestamp;
// }

void OrderBook::insertOrder(const OrderBookEntry& order) {
    std::lock_guard<std::recursive_mutex> lock(ordersMutex);
    orderMap[order.product].push_back(order);
    std::sort(orderMap[order.product].begin(), orderMap[order.product].end(), OrderBookEntry::compareByTimestamp);
}

std::vector<OrderBookEntry> OrderBook::getOrders() {
    std::lock_guard<std::recursive_mutex> lock(ordersMutex);
    
    std::vector<OrderBookEntry> allOrders;
    for (const auto& pair : orderMap) {
        allOrders.insert(allOrders.end(), pair.second.begin(), pair.second.end());
    }
    return allOrders;
}

void OrderBook::insertSales(std::vector<OrderBookEntry>& sales) {
    std::lock_guard<std::recursive_mutex> lock(ordersMutex);
    finalizedSales.insert(finalizedSales.end(), sales.begin(), sales.end());
}

std::vector<OrderBookEntry> OrderBook::getSales() {
    std::lock_guard<std::recursive_mutex> lock(ordersMutex);
    return finalizedSales;
}

void OrderBook::removeOrderById(std::size_t id) {
    std::lock_guard<std::recursive_mutex> lock(ordersMutex);
    
    for (auto& pair : orderMap) {
        auto& orders = pair.second;
        orders.erase(std::remove_if(orders.begin(), orders.end(),
            [id](const OrderBookEntry& entry) {
                return entry.id == id;
            }), orders.end());
    }
}

std::string OrderBook::getFilename() const {
    return filename;
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string currentTimestamp, UserStore& userStore) {
    std::lock_guard<std::recursive_mutex> lock(ordersMutex);
    std::unordered_map<OrderBookType, std::vector<OrderBookEntry*>> productOrders = getOrders(product);
    std::vector<OrderBookEntry*> bids = productOrders[OrderBookType::bid];
    std::vector<OrderBookEntry*> asks = productOrders[OrderBookType::ask];

    std::vector<OrderBookEntry> sales;

    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);

    for (OrderBookEntry* ask : asks){
        if (ask->amount <= 0.0) {
            continue; //ask already completely filled
        }
        for (OrderBookEntry* bid : bids){
            if (bid->amount <= 0.0) {
                continue; //bid already completely filled
            }

            if (bid->price >= ask->price) { //we have a match

                User& buyer = userStore.getUser(bid->username);
                User& seller = userStore.getUser(ask->username);

                OrderBookType type = OrderBookType::sale;
                OrderBookEntry sale{ask->price, 0.0, currentTimestamp, product, type};

                if (bid->amount == ask->amount) { //bid completely clears ask
                    sale.amount = ask->amount;
                    sales.push_back(sale);
                    bid->amount = 0.0; //make sure the bid is not processed again
                    ask->amount = 0.0;
                    processSale(buyer, seller, sale, bid->price, ask->price);
                    break; //can do no more with this ask, go onto the next ask

                } else if (bid->amount > ask->amount) { //ask is completely gone slice the bid
                    sale.amount = ask->amount;
                    sales.push_back(sale);
                    bid->amount -= ask->amount; //we adjust the bid in place so it can be used to process the next ask
                    ask->amount = 0.0;
                    processSale(buyer, seller, sale, bid->price, ask->price);
                    break; //ask is completely gone, so go to next ask

                } else if (bid->amount < ask->amount) { //bid is completely gone, slice the ask
                    sale.amount = bid->amount;
                    sales.push_back(sale);
                    ask->amount -= bid->amount; //update the ask and allow further bids to process the remaining amount
                    bid->amount = 0.0; //make sure the bid is not processed again
                    processSale(buyer, seller, sale, bid->price, ask->price);
                    continue; //some ask remains so go to the next bid

                }
            }
        }
    }
    insertSales(sales);
    removeMatchedOrders(product);
    return sales;
}

void OrderBook::processSale(User& buyer, User& seller, const OrderBookEntry& sale, double bid_price, double ask_price) {
    /** example: sale BTC/USDT
     * buyer pays USDT and receives BTC
     * seller pays BTC and receives USDT
     */
    std::vector<std::string> tokens = CSVHandler::extractTokens(sale.product, '/');
    if (tokens.size() != 2) {
        throw std::invalid_argument("Invalid product format in sale");
    }
    double totalCost = sale.price * sale.amount;

    // Update buyer's wallet
    Wallet& buyerWallet = buyer.getWallet();
    if (!buyerWallet.spendLocked(tokens[1], totalCost)) {
        if (buyer.getUsername() != "default") {
            std::cout<<"name: " << buyer.getUsername() << "wallet: "<< buyerWallet.toString() <<"required: "<< totalCost << "product: "<< sale.product << std::endl;
            throw std::runtime_error("Buyer has insufficient funds"); //TODO
        }
    }
    buyerWallet.insertCurrency(tokens[0], sale.amount);

    // Update seller's wallet
    Wallet& sellerWallet = seller.getWallet();
    if (!sellerWallet.spendLocked(tokens[0], sale.amount)) {
        if (seller.getUsername() != "default") {
            std::cout<<"name: " << seller.getUsername() << " wallet: "<< sellerWallet.toString() <<" required: "<< sale.amount << " product: "<< sale.product << std::endl;
            throw std::runtime_error("Seller has insufficient funds"); //TODO
        }
    }
    sellerWallet.insertCurrency(tokens[1], totalCost);

    // Handle refunds for buyer if bid price > ask price
    if (bid_price > ask_price) {
        double refund = (bid_price - ask_price) * sale.amount;
        buyerWallet.unlockCurrency(tokens[1], refund);
    }
}

void OrderBook::removeMatchedOrders(const std::string& product){
    std::lock_guard<std::recursive_mutex> lock(ordersMutex);

    auto it = orderMap.find(product);
    if (it == orderMap.end())
        return;

    auto& orders = it->second;

    orders.erase(
        std::remove_if(
            orders.begin(),
            orders.end(),
            [](const OrderBookEntry& entry) {
                return entry.amount <= 0.0;
            }
        ),
        orders.end()
    );
}

void OrderBook::save() {
    std::lock_guard<std::recursive_mutex> lock(ordersMutex);
    nlohmann::json j;
    j["orderMap"] = orderMap;
    j["finalizedSales"] = finalizedSales;
    
    std::ofstream outFile("orderbook.json");
    if (outFile.is_open()) {
        outFile << j.dump(4);
        outFile.close();
        std::cout << "OrderBook saved to orderbook.json" << std::endl;
    } else {
        std::cerr << "Unable to save OrderBook to orderbook.json" << std::endl;
    }
}

void OrderBook::load() {
    std::lock_guard<std::recursive_mutex> lock(ordersMutex);
    std::ifstream inFile("orderbook.json");
    if (inFile.is_open()) {
        try {
            nlohmann::json j;
            inFile >> j;
            if (j.contains("orderMap")) {
                j.at("orderMap").get_to(orderMap);
            }
            if (j.contains("finalizedSales")) {
                j.at("finalizedSales").get_to(finalizedSales);
            }
            std::cout << "OrderBook loaded from orderbook.json" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error loading OrderBook: " << e.what() << std::endl;
        }
    } else {
        std::cout << "No OrderBook json found, keeping existing data." << std::endl;
    }
}