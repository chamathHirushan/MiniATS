#pragma once
#include <string>
#include <unordered_map>
#include "User.hpp"
// #include <nlohmann/json.hpp>

class UserStore {
    public:
        UserStore(const std::string& filename);

        bool userExists(const std::string& username);
        void addUser(const std::string& username, const std::string& password);
        bool removeUser(const std::string& username);
        User& getUser(const std::string& username);

    private:
        std::string filename;
        std::unordered_map<std::string, User> users;
};