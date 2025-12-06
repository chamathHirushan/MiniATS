#include "OrderBookEntry.hpp"

OrderBookEntry:: OrderBookEntry(double price, 
                       double amount, 
                       std::string timestamp, 
                       std::string product, 
                       OrderBookType orderType)
        {
            this->price = price;
            this->amount = amount;   
            this->timestamp = timestamp;
            this->product = product;
            this->orderType = orderType;
        }