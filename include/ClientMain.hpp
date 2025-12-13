#pragma once
#include <string>
#include <iostream>
#include <asio.hpp>

class ClientMain {
    public:
        ClientMain() =default;
        void init();
        
    private:
        void run();
        asio::io_context io_context; // ASIO IO context - platform specific interface (client side)
};
