#pragma once
#include <string>
#include "Wallet.hpp"
#include <nlohmann/json.hpp>


class User {
    public:
        User() = default;
        User(const std::string& username, const std::string& password);
        std::string getUsername();
        bool validatePassword(const std::string& password);
        Wallet& getWallet();

        friend void to_json(nlohmann::json& j, const User& u);
        friend void from_json(const nlohmann::json& j, User& u);

    private:
        std::string username;
        std::string password;
        Wallet wallet;
};

void to_json(nlohmann::json& j, const User& u);
void from_json(const nlohmann::json& j, User& u);