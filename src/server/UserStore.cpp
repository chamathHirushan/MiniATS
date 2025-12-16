#include "UserStore.hpp"
#include <stdexcept>

UserStore::UserStore(const std::string& filename) {
    this->filename = filename;
}

bool UserStore::userExists(const std::string& username) {
    return users.find(username) != users.end();
}

void UserStore::addUser(const std::string& username, const std::string& password) {
    users.emplace(username, User(username, password));
}

bool UserStore::removeUser(const std::string& username) {
    return users.erase(username) > 0;
}

User& UserStore::getUser(const std::string& username) {
    auto it = users.find(username);
    if (it != users.end()) {
        return it->second;
    }
    throw std::runtime_error("User not found: " + username);
}