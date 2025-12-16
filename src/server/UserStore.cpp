#include "UserStore.hpp"

UserStore::UserStore() {}

void UserStore::init(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open user store file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string username, password;
        
        if (std::getline(ss, username, ',') && std::getline(ss, password)) {
            if (!password.empty() && password.back() == '\r') {
                password.pop_back();
            }
            users[username] = password;
        }
    }
    std::cout << "Loaded " << users.size() << " users." << std::endl;
}

bool UserStore::validate(const std::string& username, const std::string& password) {
    auto it = users.find(username);
    if (it != users.end()) {
        if (it->second == password) {
            return true;
        }
    }
    return false;
}

bool UserStore::userExists(const std::string& username) {
    return users.find(username) != users.end();
}

void UserStore::addUser(const std::string& username, const std::string& password) {
    users[username] = password;
}