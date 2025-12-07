#include <string>
#include <vector>
#include "OrderBookEntry.hpp"
#include "CSVReader.hpp"
#include <iostream>
#include <fstream>

std::vector<std::string> CSVReader::extractTokens(const std::string& csvLine, char separator=',') {
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

OrderBookEntry CSVReader::parseLine(const std::vector<std::string>& tokens) {
    if (tokens.size() != 5) {
        std::cout << "Invalid line. Skipping..." << std::endl;
        throw std::exception{};
    }
    
    try{
        std::string timestamp = tokens[0];
        std::string product = tokens[1];
        OrderBookType orderType = OrderBookEntry::determineOrderType(tokens[2]);
        double price = std::stod(tokens[3]);
        double amount = std::stod(tokens[4]);

        OrderBookEntry entry(price, amount, timestamp, product, orderType);
        return entry;
    } catch (const std::exception& e) {
        std::cout << "Error parsing line: " << e.what() << ". Skipping..." << std::endl;
        throw;
    }
}

std::vector<OrderBookEntry> CSVReader::readCSV(const std::string& filename) {
    std::vector<OrderBookEntry> entries;
    // Implementation for reading CSV file and populating entries
    std::ifstream csvFile{filename};
    if (!csvFile.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        throw std::exception{};
    }

    std::string line;
    while(std::getline(csvFile, line)) {
        std::vector<std::string> tokens = extractTokens(line);
        OrderBookEntry entry = parseLine(tokens);
        std::cout << "Parsed entry: Price = " << entry.price << ", Amount = " << entry.amount << std::endl;
        entries.push_back(entry);
    }
    csvFile.close();

    return entries;
}