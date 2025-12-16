#pragma once
#include "OrderBook.hpp"
#include "Wallet.hpp"
#include <string>
#include <vector>
#include <memory>
#include <asio.hpp>
#include "UserStore.hpp"
#include <map>

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

        OrderBook orderBook{"orders.csv"};
        UserStore userStore{"users.csv"};//TODO
        std::map<std::string, Wallet> wallets;
        bool isRunning = true; // Flag to control server running state

        static void cleanup(int signum); // Signal handler for cleanup
        static ServerMain* serverInstance; // Pointer to current instance for signal handling

        asio::io_context io_context; // ASIO IO context - platform specific interface (server side)
        std::unique_ptr<tcp::acceptor> acceptor; //// store a pointer to a acceptor that accepts incoming client connections using TCP
};
