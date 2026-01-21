#pragma once
#include "OrderBook.hpp"
#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <asio.hpp>
#include "UserStore.hpp"
#include <map>
#include <mutex>
#include <condition_variable>
#include <future>

using asio::ip::tcp;

class ServerMain {
    public:
        ServerMain() = default;
        void init();
        void run();
        
    private:
        void handleClient(std::shared_ptr<tcp::socket> clientSocket);
        void startMatching();
        void startMatchingProduct(std::string product);
        std::string getCurrentTimestamp();

        OrderBook orderBook{"orders.csv"};
        UserStore userStore{"users.json"};
        bool isRunning = true; // Flag to control server running state

        static void cleanup(int signum); // Signal handler for cleanup
        static ServerMain* serverInstance; // Pointer to current instance for signal handling

        asio::io_context io_context; // ASIO IO context - platform specific interface (server side)
        std::unique_ptr<tcp::acceptor> acceptor; //// store a pointer to a acceptor that accepts incoming client connections using TCP

        std::mutex matchingMutex;
        std::condition_variable matchingCV; // Condition variable for matching engine thread
        std::deque<std::string> pendingProducts; // pending products for matching

        std::vector<std::future<void>> matchingFutures; // Store matching engine threads

        void startRestServer(); // Method to start Crow REST API
        std::thread restThread; // Thread to run the REST API
};
