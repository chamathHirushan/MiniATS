#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>

class UserStore {
    public:
        UserStore();
        void init(const std::string& filename);
        bool validate(const std::string& username, const std::string& password);

    private:
        std::unordered_map<std::string, std::string> users;
};
