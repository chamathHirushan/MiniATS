#include <string>
#include <vector>
#include <iostream>
#include <fstream>

std::vector<std::string> extractWords(const std::string csvLine, char separator = ',') {
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
        std::vector<std::string> words = extractWords(line);
        for (const std::string& word : words) {
            std::cout << word << " | ";
        }
        std::cout << std::endl;
    }
    csvFile.close();
}