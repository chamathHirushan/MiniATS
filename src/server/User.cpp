#include "User.hpp"
#include "Wallet.hpp"
#include <string>

User::User(const std::string& username, const std::string& password) {
    this->username = username;
    this->password = password;
}

std::string User::getUsername() {
    return username;
}

bool User::validatePassword(const std::string& password) {
    return this->password == password;
}

Wallet& User::getWallet() {
    return wallet;
}

void to_json(nlohmann::json& j, const User& u) {
    j = nlohmann::json{
        {"username", u.username},
        {"password", u.password},
        {"wallet", u.wallet}
    };
}

void from_json(const nlohmann::json& j, User& u) {
    j.at("username").get_to(u.username);
    j.at("password").get_to(u.password);
    if (j.contains("wallet")) {
        j.at("wallet").get_to(u.wallet);
    }
}