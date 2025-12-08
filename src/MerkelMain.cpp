#include "MerkelMain.hpp"
#include <iostream>
#include "OrderBookEntry.hpp"

MerkelMain::MerkelMain() {
}

void MerkelMain::init() {
    currentTimestamp = orderBook.getEarliestTimestamp();
    while (true) {
        printMenu();
        int userOption = getUserOption();
        processUserOption(userOption);
    }
};

void MerkelMain::printMenu() {
    std::cout << std::endl;
    std::cout << "1: Print help" << std::endl;
    std::cout << "2: Print exchange stats" << std::endl;
    std::cout << "3: Place an ask" << std::endl;
    std::cout << "4: Place a bid" << std::endl;
    std::cout << "5: Print wallet" << std::endl;
    std::cout << "6: Continue" << std::endl;
    std::cout<< "Current time is : " << currentTimestamp << std::endl;
};

int MerkelMain::getUserOption() {
    int userOption;
    std::cout << "Type in 1-6" << std::endl;
    std::cout << "===========" << std::endl;
    std::cin >> userOption;
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
};

void MerkelMain::printHelp() {
    std::cout << "Help - choose options from the menu" << std::endl;
    std::cout << "and follow the on screen instructions." << std::endl;
};
    
void MerkelMain::printMarketStats() {

    for (const std::string& product : orderBook.getKnownProducts()) {
        std::cout << "Product: " << product << std::endl;
        std::vector<OrderBookEntry> bids = orderBook.getOrders(OrderBookType::bid, product, "2020/03/17 17:01:24.884492");
        std::cout<< "  Bids: " << bids.size() << std::endl;
        std::cout << "  High Bid: " << OrderBook::getHighPrice(bids) << std::endl;
        std::vector<OrderBookEntry> asks = orderBook.getOrders(OrderBookType::ask, product, "2020/03/17 17:01:24.884492");
        std::cout<< "  Asks: " << asks.size() << std::endl;
        std::cout << "  Low Ask: " << OrderBook::getLowPrice(asks) << std::endl;
    }
    // std::cout << "OrderBook contains : " << orderBook.entries.size() << " entries." << std::endl;
    // unsigned int bid =0;
    // unsigned int ask =0;
    // for (const OrderBookEntry& e : entries) {
    //     if (e.orderType == OrderBookType::bid) {
    //         bid++;
    //     } else if (e.orderType == OrderBookType::ask) {
    //         ask++;
    //     }
    // }
    // std::cout << "Bids: " << bid << ", Asks: " << ask << std::endl;
};

void MerkelMain::enterAsk() {
    std::cout << "You selected: Place an ask" << std::endl;
};

void MerkelMain::enterBid() {
    std::cout << "You selected: Place a bid" << std::endl;
};

void MerkelMain::printWallet() {
    std::cout << "You selected: Print wallet" << std::endl;
};

void MerkelMain::gotoNextTimeframe() {
    std::cout << "You selected: Continue" << std::endl;
};

void MerkelMain::processUserOption(int userOption) {
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
};