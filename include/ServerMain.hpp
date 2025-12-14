#pragma once
#include "OrderBook.hpp"
#include "Wallet.hpp"
#include <string>
#include <vector>
#include <memory>
#include <asio.hpp>

using asio::ip::tcp;

class ServerMain {
    public:
        ServerMain() = default;
        void init();
        void run();
        
    private:
        void handleClient(std::shared_ptr<tcp::socket> clientSocket);
        void startMatching();
        std::string getCurrentTimestamp();

        OrderBook orderBook{"20200317.csv"};
        Wallet wallet;
        bool isRunning = true; // Flag to control server running state

        asio::io_context io_context; // ASIO IO context - platform specific interface (server side)
        std::unique_ptr<tcp::acceptor> acceptor; //// store a pointer to a acceptor that accepts incoming client connections using TCP
};
