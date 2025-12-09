#pragma once
#include <string>
#include <map>

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

    private:
        std::map<std::string, double> currencies;
};