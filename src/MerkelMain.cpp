#include "MerkelMain.hpp"
#include <iostream>
#include "OrderBookEntry.hpp"
#include "CSVReader.hpp"
#include <limits>

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
    std::string line;
    std::cout << "Type in 1-6" << std::endl;
    std::cout << "===========" << std::endl;
    std::getline(std::cin, line);
    try{
        int userOption = std::stoi(line);
        return userOption;
    } catch (const std::exception& e) {
        return 0;
    }
};

void MerkelMain::printHelp() {
    std::cout << "Help - choose options from the menu" << std::endl;
    std::cout << "and follow the on screen instructions." << std::endl;
};
    
void MerkelMain::printMarketStats() {

    for (const std::string& product : orderBook.getKnownProducts()) {
        std::cout << "Product: " << product << std::endl;
        std::vector<OrderBookEntry> bids = orderBook.getOrders(OrderBookType::bid, product, currentTimestamp);
        std::cout<< "  Bids: " << bids.size() << std::endl;
        std::cout << "  High Bid: " << OrderBook::getHighPrice(bids) << std::endl;
        std::cout << "  Avg Bid Price: " << OrderBook::getAvgPrice(bids) << std::endl;
        std::cout << "  Total Bid Volume: " << OrderBook::getTotalVolume(bids) <<  std::endl;
        
        std::vector<OrderBookEntry> asks = orderBook.getOrders(OrderBookType::ask, product, currentTimestamp);
        std::cout<< "  Asks: " << asks.size() << std::endl;
        std::cout << "  Low Ask: " << OrderBook::getLowPrice(asks) << std::endl;
        std::cout << "  Avg Ask Price: " << OrderBook::getAvgPrice(asks) << std::endl;
        std::cout << "  Total Ask Volume: " << OrderBook::getTotalVolume(asks) << std::endl;
    }
};

void MerkelMain::enterAsk() {
    std::cout << "Place an ask, enter - product, price, amount  eg: ETH/BTC,100.5,2" << std::endl;
    std::string askOffer;
    std::getline(std::cin, askOffer);

    std::vector<std::string> tokens = CSVReader::extractTokens(askOffer, ',');
    if (tokens.size() != 3) {
        std::cout << "Invalid ask format. Please use product,price,amount" << std::endl;
        return;
    }
    try{
        OrderBookEntry newAsk = CSVReader::parseLine(tokens[1], tokens[2], currentTimestamp, tokens[0], OrderBookType::ask);
        orderBook.insertOrder(newAsk);
        std::cout << "You entered: " << askOffer << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bad ask format. Please use product,price,amount" << std::endl;
    }

};

void MerkelMain::enterBid() {
    std::cout << "Place a bid, enter - product, price, amount   eg: ETH/BTC,100.5,2" << std::endl;
    std::string bidOffer;
    std::getline(std::cin, bidOffer);

    std::vector<std::string> tokens = CSVReader::extractTokens(bidOffer, ',');
    if (tokens.size() != 3) {
        std::cout << "Invalid bid format. Please use product,price,amount" << std::endl;
        return;
    }
    try{
        OrderBookEntry newBid = CSVReader::parseLine(tokens[1], tokens[2], currentTimestamp, tokens[0], OrderBookType::bid);
        orderBook.insertOrder(newBid);
        std::cout << "You entered: " << bidOffer << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Bad bid format. Please use product,price,amount" << std::endl;
    }
};

void MerkelMain::printWallet() {
    std::cout << "You selected: Print wallet" << std::endl;
};

void MerkelMain::gotoNextTimeframe() {
    std::cout << "You selected: Continue" << std::endl;
    std::string nextTimestamp = orderBook.getNextTimestamp(currentTimestamp);
    std::vector<OrderBookEntry> newSales = orderBook.matchAsksToBids("ETH/BTC", currentTimestamp);
    for (const OrderBookEntry& sale : newSales) {
        std::cout << "Sale: " << sale.product << " Price: " << sale.price << " Amount: " << sale.amount << std::endl;
    }
    std::cout<<"Total new sales: " << newSales.size() << std::endl;
    if (nextTimestamp != "") {
        currentTimestamp = nextTimestamp;
    } else {
        std::cout << "No more data available. Staying at current timestamp." << std::endl;
    }
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