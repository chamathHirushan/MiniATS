#include "MerkelMain.hpp"
#include <iostream>
#include "OrderBookEntry.hpp"

MerkelMain::MerkelMain() {
}

void MerkelMain::init() {
    loadOrderBook();
    while (true) {
        printMenu();
        int userOption = getUserOption();
        processUserOption(userOption);
    }
};

void MerkelMain::loadOrderBook() {
    std::cout << "Loading order book..." << std::endl;

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
};

void MerkelMain::printMenu() {
    std::cout << std::endl;
    std::cout << "1: Print help" << std::endl;
    std::cout << "2: Print exchange stats" << std::endl;
    std::cout << "3: Place an ask" << std::endl;
    std::cout << "4: Place a bid" << std::endl;
    std::cout << "5: Print wallet" << std::endl;
    std::cout << "6: Continue" << std::endl;
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
    std::cout << "OrderBook contains : " << entries.size() << " entries." << std::endl;
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