#pragma once
#include <string>
#include <map>
#include <mutex>
#include "OrderBookEntry.hpp"

class Wallet{
    public:
        Wallet() = default;
        Wallet(const Wallet& other); // Custom copy constructor for saving/loading
        Wallet& operator=(const Wallet& other); // Custom copy assignment operator for saving/loading
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
        /** Unlocks a specified amount of a given currency type in the wallet */
        bool unlockCurrency(const std::string& type, double amount);

    private:
        std::map<std::string, double> currencies;
        std::map<std::string, double> locked;
        mutable std::recursive_mutex mtx;

        /** Locks a specified amount of a given currency type in the wallet */
        bool lockCurrency(const std::string& type, double amount);

        friend void to_json(nlohmann::json& j, const Wallet& w);
        friend void from_json(const nlohmann::json& j, Wallet& w);
};