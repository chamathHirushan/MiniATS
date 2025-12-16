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