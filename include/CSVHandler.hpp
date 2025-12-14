#pragma once
#include <string>
#include <vector>
#include "OrderBookEntry.hpp"

class CSVHandler {
    public:
        CSVHandler()= default;
        static std::vector<OrderBookEntry> readCSV(const std::string& filename);
        static void appendEntriesToCSV(const std::string& filename, const std::vector<OrderBookEntry>& entries);
        static std::vector<std::string> extractTokens(const std::string& csvLine, char separator);
        static OrderBookEntry parseLine(std::string price, std::string amount, std::string timestamp, std::string product, OrderBookType orderType);
    private:
        static OrderBookEntry parseLine(const std::vector<std::string>& tokens);
};