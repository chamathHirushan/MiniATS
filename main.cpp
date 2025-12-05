#include <iostream>
#include <string>
#include <vector>

enum class OrderBookType {bid, ask};

class OrderBookEntry {
    public:
        OrderBookEntry(double _price, 
                       double _amount, 
                       std::string _timestamp, 
                       std::string _product, 
                       OrderBookType _orderType)
        : price(_price), 
          amount(_amount), 
          timestamp(_timestamp), 
          product(_product), 
          orderType(_orderType)
        {
            
        }

        double price;
        double amount;
        std::string timestamp;
        std::string product;
        OrderBookType orderType;
};

void printMenu() {
    std::cout << "1: Print help" << std::endl;
    std::cout << "2: Print exchange stats" << std::endl;
    std::cout << "3: Place an ask" << std::endl;
    std::cout << "4: Place a bid" << std::endl;
    std::cout << "5: Print wallet" << std::endl;
    std::cout << "6: Continue" << std::endl;
}

int getUserOption() {
    int userOption;
    std::cout << "Type in 1-6" << std::endl;
    std::cin >> userOption;
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void printHelp() {
    std::cout << "Help - choose options from the menu" << std::endl;
    std::cout << "and follow the on screen instructions." << std::endl;
}

void printMarketStats() {
    std::cout << "You selected: Print exchange stats" << std::endl;
}

void enterAsk() {
    std::cout << "You selected: Place an ask" << std::endl;
}

void enterBid() {
    std::cout << "You selected: Place a bid" << std::endl;
}

void printWallet() {
    std::cout << "You selected: Print wallet" << std::endl;
}

void gotoNextTimeframe() {
    std::cout << "You selected: Continue" << std::endl;
}

void processUserOption(int userOption) {
    switch (userOption) {
        case 1:
            printHelp();
            break;
        case 2:
            printMarketStats();
            break;
        case 3:
            enterAsk();
            break;
        case 4:
            enterBid();
            break;
        case 5:
            printWallet();
            break;
        case 6:
            gotoNextTimeframe();
            break;
        default:
            std::cout << "Invalid choice. Please enter a number between 1 and 6." << std::endl;
            break;
    }
}

double computeAveragePrice(std::vector<OrderBookEntry>& entries) {
    double sum = 0;
    for (const OrderBookEntry& e : entries) {
        sum += e.price;
    }
    return sum / entries.size();
}

double computeLowPrice(std::vector<OrderBookEntry>& entries) {
    double min = entries[0].price;
    for (const OrderBookEntry& e : entries) {
        if (e.price < min) {
            min = e.price;
        }
    }
    return min;
}

double computeHighPrice(std::vector<OrderBookEntry>& entries) {
    double max = entries[0].price;
    for (const OrderBookEntry& e : entries) {
        if (e.price > max) {
            max = e.price;
        }
    }
    return max;
}

double computePriceSpread(std::vector<OrderBookEntry>& entries) {
    return computeHighPrice(entries) - computeLowPrice(entries);
}

int main() {
    std::vector<OrderBookEntry> entries;

    entries.push_back(OrderBookEntry(1000.0, 
                                     0.02, 
                                     "2020/03/17 17:01:24.884492", 
                                     "BTC/USDT", 
                                     OrderBookType::bid));

    entries.push_back(OrderBookEntry(2000.0, 
                                     0.02, 
                                     "2020/03/17 17:01:24.884492", 
                                     "BTC/USDT", 
                                     OrderBookType::ask));

    for (OrderBookEntry& e : entries) {
        std::cout << "Price: " << e.price << std::endl;
    }

    std::cout << "Average Price: " << computeAveragePrice(entries) << std::endl;
    std::cout << "Low Price: " << computeLowPrice(entries) << std::endl;
    std::cout << "High Price: " << computeHighPrice(entries) << std::endl;
    std::cout << "Price Spread: " << computePriceSpread(entries) << std::endl;

    while (true) {
        printMenu();
        int userOption = getUserOption();
        processUserOption(userOption);
    }
    return 0;
}
