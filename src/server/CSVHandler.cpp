#include <string>
#include <vector>
#include "OrderBookEntry.hpp"
#include "CSVHandler.hpp"
#include <iostream>
#include <fstream>

std::vector<std::string> CSVHandler::extractTokens(const std::string& csvLine, char separator=',') {
    // Implementation for extracting tokens from a CSV line
    std::vector<std::string> tokens;
    signed int start, end;
    
    start = csvLine.find_first_not_of(separator, 0);
    do{
        end = csvLine.find_first_of(separator, start);
        if (start == csvLine.length() || start == end) break;
        if (end >= 0) {
            tokens.push_back(csvLine.substr(start, end - start));
            start = end + 1;
        } else {
            tokens.push_back(csvLine.substr(start, csvLine.length() - start));
            break;
        }
    }while(true);
    return tokens;
}

OrderBookEntry CSVHandler::parseLine(std::string price, std::string amount, std::string timestamp, std::string product, OrderBookType orderType) {
    try{   
        double priceValue = std::stod(price);
        double amountValue = std::stod(amount);
        OrderBookEntry entry{priceValue, amountValue, timestamp, product, orderType};
        return entry;
    } catch (const std::exception& e) {
        throw;
    }
}

OrderBookEntry CSVHandler::parseLine(const std::vector<std::string>& tokens) {
    if (tokens.size() != 5) {
        throw std::exception{};
    }

    std::string timestamp = tokens[0];
    std::string product = tokens[1];
    OrderBookType orderType = OrderBookEntry::determineOrderType(tokens[2]);
    double price = std::stod(tokens[3]);
    double amount = std::stod(tokens[4]);

    OrderBookEntry entry{price, amount, timestamp, product, orderType};
    return entry;
}

std::vector<OrderBookEntry> CSVHandler::readCSV(const std::string& filename) {
    std::vector<OrderBookEntry> entries;
    // Implementation for reading CSV file and populating entries
    std::ifstream csvFile{filename};
    std::string line;

    if (csvFile.is_open()) {
        while(std::getline(csvFile, line)) {
            std::vector<std::string> tokens = extractTokens(line);
            try{
                OrderBookEntry entry = parseLine(tokens);
                entries.push_back(entry);
            } catch(std::exception& e) {
                std::cout << "Error parsing line. Skipping..." << e.what() << std::endl;
                continue;
            }
        }
    }
    csvFile.close();

    return entries;
}