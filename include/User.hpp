#pragma once
#include <string>
#include "Wallet.hpp"

class User {
    public:
        User(const std::string& username, const std::string& password);
        std::string getUsername();
        bool validatePassword(const std::string& password);
        Wallet& getWallet();

    private:
        std::string username;
        std::string password;
        Wallet wallet;
};