#pragma once
#include <string>
#include <vector>
#include "OrderBookEntry.hpp"

class CSVReader {
    public:
        CSVReader()= default;
        static std::vector<OrderBookEntry> readCSV(const std::string& filename);
    private:
        static std::vector<std::string> extractTokens(const std::string& csvLine, char separator);
        static OrderBookEntry parseLine(const std::vector<std::string>& tokens);
};