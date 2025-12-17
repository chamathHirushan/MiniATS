#include "UserStore.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

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
    if (username == "default") { //TODO
        users.emplace(username, User(username, "default"));
        return users.at(username);
    }
    throw std::runtime_error("User not found: " + username);
}

void UserStore::save() {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        nlohmann::json j = users;
        outFile << j.dump(4);
        outFile.close();
        std::cout << "UserStore saved to " << filename << std::endl;
    } else {
        std::cerr << "Unable to save UserStore to " << filename << std::endl;
    }
}

void UserStore::load() {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        try {
            nlohmann::json j;
            inFile >> j;
            users = j.get<std::unordered_map<std::string, User>>();
            std::cout << "UserStore loaded from " << filename << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error loading UserStore: " << e.what() << std::endl;
        }
    } else {
        std::cout << "No UserStore file found at " << filename << ", starting fresh." << std::endl;
    }
}