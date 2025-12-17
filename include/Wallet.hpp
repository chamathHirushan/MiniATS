#pragma once
#include <string>
#include <map>
#include "OrderBookEntry.hpp"

class Wallet{
    public:
        Wallet() = default;
        /** Inserts a specified amount of a given currency type into the wallet */
        void insertCurrency(std::string type, double amount);
        /** Removes a specified amount of a given currency type from the wallet */
        bool removeCurrency(std::string type, double amount);
        /** Checks if the wallet contains at least the specified amount of a given currency type */
        bool containsAmmount(std::string type, double amount);
        /** Returns a string representation of the wallet */
        std::string toString();
        
        /** Checks if the wallet can fulfill the given order */
        bool fulfillOrder(OrderBookEntry& order);
        /** Cancels the given order and unlocks the associated funds in the wallet */
        bool cancelOrder(OrderBookEntry& order);
        /** Spends a specified amount of locked currency of a given type */
        bool spendLocked(const std::string& type, double amount);

    private:
        std::map<std::string, double> currencies;
        std::map<std::string, double> locked;

        /** Locks a specified amount of a given currency type in the wallet */
        bool lockCurrency(const std::string& type, double amount);
         /** Unlocks a specified amount of a given currency type in the wallet */
        bool unlockCurrency(const std::string& type, double amount);
};