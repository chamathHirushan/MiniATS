#pragma once
#include <vector>
#include "OrderBookEntry.hpp"

class MerkelMain{
    public:
        MerkelMain();
        void init();
    private:
        void loadOrderBook();
        void printMenu();
        int getUserOption();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void gotoNextTimeframe();
        void processUserOption(int userOption);

        std::vector<OrderBookEntry> entries;
};