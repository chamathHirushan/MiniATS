#pragma once
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include "OrderBookEntry.hpp"

class CSVHandler {
    public:
        CSVHandler()= default;
        static std::unordered_map<std::string, std::list<OrderBookEntry>> readCSV(const std::string& filename);
        static void entriesToCSV(const std::string& filename, const std::vector<OrderBookEntry>& entries, bool append=true);
        static std::vector<std::string> extractTokens(const std::string& csvLine, char separator);

        /** convert orderBookEntry values to an OrderBookEntry */
        static OrderBookEntry parseLine(std::string price, std::string amount, std::string timestamp, std::string product, OrderBookType orderType);
    private:
        /** convert a vector of tokens to an OrderBookEntry */
        static OrderBookEntry parseLine(const std::vector<std::string>& tokens);
};