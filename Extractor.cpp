#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "extractor.hpp"
#include "OrderBookEntry.hpp"

std::vector<std::string> extractTokens(const std::string& csvLine, char separator) {
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

void test(){
    std::ifstream csvFile{"20200317.csv"};
    std::string line;

    if (!csvFile.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return;
    }
    while(std::getline(csvFile, line)) {
        std::cout << "Read line: " << line << std::endl;
        std::vector<std::string> words = extractTokens(line);
        if (words.size() != 5) {
            std::cout << "Invalid line. Skipping..." << std::endl;
            continue;
        }
        
        try{
            std::string timestamp = words[0];
            std::string product = words[1];
            OrderBookType orderType = (words[2] == "bid") ? OrderBookType::bid : OrderBookType::ask;
            double price = std::stod(words[3]);
            double amount = std::stod(words[4]);

            std::cout << "Parsed values:" << std::endl;
            std::cout << "Price: " << price << std::endl;
            std::cout << "Amount: " << amount << std::endl;
            std::cout << "Timestamp: " << timestamp << std::endl;
            std::cout << "Product: " << product << std::endl;
            std::cout << "Order Type: " << ((orderType == OrderBookType::bid) ? "bid" : "ask") << std::endl;
            std::cout << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error parsing line: " << e.what() << ". Skipping..." << std::endl;
            continue;
        }
    }
    csvFile.close();
}