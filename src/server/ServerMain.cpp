#include "ServerMain.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <ctime>
#include <csignal>
#include "CSVHandler.hpp"

ServerMain* ServerMain::serverInstance = nullptr; // Global pointer to current instance

void ServerMain::cleanup(int signum) {
    std::cout << "\nShutting down server..." << std::endl;
    if (serverInstance != nullptr) {
        
        //CSVHandler::usersToCSV("users.csv", serverInstance->userStore.getUsers());
        CSVHandler::entriesToCSV("sales.csv", serverInstance->orderBook.getSales());
        CSVHandler::entriesToCSV(serverInstance->orderBook.getFilename(), serverInstance->orderBook.getOrders(), false);
        std::cout << "Exported records successfully." << std::endl;

        serverInstance->isRunning = false;
        if (serverInstance->acceptor) {
            serverInstance->acceptor->close();
        }
        serverInstance->io_context.stop();
    }
    std::exit(signum);
}

void ServerMain::init() {
    std::cout << "ServerMain::init() starting..." << std::endl;
    serverInstance = this;
    std::signal(SIGINT, ServerMain::cleanup); // Handle Ctrl+C
    std::signal(SIGTERM, ServerMain::cleanup); // Handle termination signal
    
    try {
        // Create and bind the TCP acceptor to listen on port 5322 (IPv4) for incoming client connections
        acceptor = std::make_unique<tcp::acceptor>(io_context, tcp::endpoint(tcp::v4(), 5322));
    } catch (std::exception& e) {
        std::cerr << "Exception in init: " << e.what() << std::endl;
        return;
    }

    // starting the matching engine in a separate thread
    std::thread matchingEngine(&ServerMain::startMatching, this);
    matchingEngine.detach();
    std::cout << "Server started." << std::endl;
    
    run();
}

void ServerMain::run() {
    std::cout << "Server listening on port 5322..." << std::endl;

    while(isRunning) {
        try {
            auto socket = std::make_shared<tcp::socket>(io_context);
            acceptor->accept(*socket); /// Block until a new client connects, then store the connection in 'socket'
            
            std::cout << "Client connected!" << std::endl;
            // Spawn thread - pass this to refer the current ServerMain object, and the client socket
            std::thread clientThread(&ServerMain::handleClient, this, socket);
            // Detach the thread so it runs independently; the server can continue accepting other clients
            clientThread.detach();
        } catch (std::exception& e) {
            if (isRunning) {
                std::cerr << "Exception in run: " << e.what() << std::endl;
            }
        }
    }
}

void ServerMain::handleClient(std::shared_ptr<tcp::socket> clientSocket) {
    try {
        std::string username = "";
        
        // Send welcome message
        std::string welcome = "Welcome to MiniATS Server\n";
        asio::write(*clientSocket, asio::buffer(welcome));

        char data[512];
        
        while (true) {
            asio::error_code error;
            size_t length = clientSocket->read_some(asio::buffer(data), error);
            
            if (error == asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw asio::system_error(error); // Some other error.

            std::string msg(data, length);
            // Remove newline if present for cleaner logging
            while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r')) {
                msg.pop_back();
            }
            
            std::cout << "Bytes received: " << length << " Msg: " << msg << std::endl;

            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream{msg};
            while (std::getline(tokenStream, token, ' ')) { // split by 
                tokens.push_back(token);
            }

            std::string response;
            if (tokens.empty()) {
                continue;
            }

            std::string command = tokens[0]; // First token is the command
            std::transform(command.begin(), command.end(), command.begin(),
                   [](unsigned char c){ return std::toupper(c); });
            
            if (command == "REGISTER") {
                if (tokens.size() == 3) {
                    std::string u = tokens[1];
                    std::string p = tokens[2];

                    if (u == "default") {
                        response = "ERR Username not allowed. Please choose another.";
                    }
                    else if (userStore.userExists(u)) {
                        response = "ERR User already exists. Please LOGIN.";
                    } else {
                        userStore.addUser(u, p);
                        username = u;
                        response = "OK User registered as " + username;
                    }
                } else {
                    response = "ERR Invalid REGISTER command";
                }
            }
            else if (command == "LOGIN") {
                if (tokens.size() == 3) {
                    std::string u = tokens[1];
                    std::string p = tokens[2];

                    if (u == "default") {
                        response = "ERR Username not allowed. Please choose another.";
                    }
                    else if (userStore.userExists(u)) {
                        User& user = userStore.getUser(u);
                        if (!user.validatePassword(p)) {
                            response = "ERR Invalid credentials. Try again.";
                            asio::write(*clientSocket, asio::buffer(response + "\n"));
                            continue;
                        }
                        username = u;
                        response = "OK Logged in as " + username;
                    } else {
                        response = "ERR Invalid username. Please REGISTER first.";
                    }
                } else {
                    response = "ERR Invalid LOGIN command";
                }
            }
            else if (command == "ASK" || command == "BID") {
                if (username.empty()) {
                    response = "ERR Login required";
                } else if (tokens.size() == 4) {
                    std::string product = tokens[1];
                    std::transform(product.begin(), product.end(), product.begin(),
                       [](unsigned char c){ return std::toupper(c); });
                    double amount = 0;
                    double price = 0;
                    try {
                        amount = std::stod(tokens[2]);
                        price = std::stod(tokens[3]);
                        
                        OrderBookType type = (command == "ASK") ? OrderBookType::ask : OrderBookType::bid;
                        OrderBookEntry obe(price, amount, getCurrentTimestamp(), product, type, username);
                        if (!userStore.getUser(username).getWallet().fulfillOrder(obe)) {
                            response = "ERR Insufficient funds in wallet to place this " + command + ".";
                            asio::write(*clientSocket, asio::buffer(response + "\n"));
                            continue;
                        }
                        orderBook.insertOrder(obe);
                        
                        response = "OK " + command + " placed for " + product;
                    } catch (const std::exception& e) {
                        response = "ERR Invalid order";
                    }
                } else {
                    response = "ERR Invalid " + command + " command";
                }
            }else if (command == "ORDERS") {
                if (username.empty()) {
                    response = "ERR Login required";
                } else {
                    std::ostringstream oss;
                    std::vector<OrderBookEntry> orders = orderBook.getOrders();
                    int count = 0;
                    oss << "Ongoing orders of user " << username << ":\n";
                    for (const auto& order : orders) {
                        if (order.username == username) {
                            count++;
                            oss << " ID: " << order.id
                                << " | " << OrderBookEntry::orderTypeToString(order.orderType)
                                << " | " << order.product
                                << " | Amount: " << order.amount
                                << " | Price: " << order.price
                                << " | Timestamp: " << order.timestamp << "\n";
                        }
                    }
                    if (count == 0) {
                        oss << "    (no ongoing orders)";
                    }
                    response = oss.str();
                }
            }
            else if (command == "CANCEL") {
                if (username.empty()) {
                    response = "ERR Login required";
                } else if (tokens.size() == 2) {
                    int orderId = 0;
                    try {
                        orderId = std::stoi(tokens[1]);
                        std::vector<OrderBookEntry> orders = orderBook.getOrders();
                        bool found = false;
                        for (auto& order : orders) {
                            if (orderId == order.id && order.username == username) {
                                found = true;
                                userStore.getUser(username).getWallet().cancelOrder(order);
                                break;
                            }
                        }
                        if (found) {
                            orderBook.removeOrderById(orderId);
                            response = "OK Order cancelled.";
                        } else {
                            response = "ERR Order not found or not owned by user.";
                        }
                    } catch (const std::exception& e) {
                        response = "ERR Invalid order ID";
                    }
                } else {
                    response = "ERR Invalid CANCEL command";
                }
            }
            else if (command == "WALLET") {
                if (username.empty()) {
                    response = "ERR Login required";
                } else {
                    response = userStore.getUser(username).getWallet().toString();
                }
            }
            else if (command == "DEPOSIT" || command == "WITHDRAW") {
                if (username.empty()) {
                    response = "ERR Login required";
                } else if (tokens.size() == 3) {
                    std::string product = tokens[1];
                    std::transform(product.begin(), product.end(), product.begin(),
                       [](unsigned char c){ return std::toupper(c); });
                    double amount = 0;
                    try {
                        amount = std::stod(tokens[2]);
                        if (command == "WITHDRAW") {
                            if (userStore.getUser(username).getWallet().removeCurrency(product, amount)) {
                                response = "OK Withdrew " + std::to_string(amount) + " " + product;
                            } else {
                                response = "ERR Insufficient funds";
                            }
                        } else {
                            userStore.getUser(username).getWallet().insertCurrency(product, amount);
                            response = "OK Deposited " + std::to_string(amount) + " " + product;
                        }
                    } catch (const std::exception& e) {
                        response = "ERR Invalid amount";
                    }
                }else {
                    response = "ERR Invalid " + command + " command";
                }
            }
            else if (command == "MARKET") {
                std::ostringstream oss;
                for (const auto& product : orderBook.getKnownProducts()) {
                    oss << "\n Product: " << product << "\t Best bid: "
                        << OrderBook::getHighPrice(orderBook.getOrders(OrderBookType::bid, product))
                        << "\t Best ask: "
                        << OrderBook::getLowPrice(orderBook.getOrders(OrderBookType::ask, product));
                }
                response = oss.str();
            }
            else if (command == "STATS") {
                if (tokens.size() == 2) {
                    std::string product = tokens[1];
                    std::transform(product.begin(), product.end(), product.begin(),
                       [](unsigned char c){ return std::toupper(c); });
                    std::ostringstream oss;

                    // ----- BIDS -----
                    std::vector<OrderBookEntry*> bids =orderBook.getOrders(OrderBookType::bid, product);
                    oss << "\n  Bids \t\t\t: " << bids.size() << "\n";
                    oss << "  High Bid \t\t: " << OrderBook::getHighPrice(bids) << "\n";
                    oss << "  Avg Bid Price \t: " << OrderBook::getAvgPrice(bids) << "\n";
                    oss << "  Total Bid Volume \t: " << OrderBook::getTotalVolume(bids) << "\n";

                    // ----- ASKS -----
                    std::vector<OrderBookEntry*> asks = orderBook.getOrders(OrderBookType::ask, product);
                    oss << "\n  Asks \t\t\t: " << asks.size() << "\n";
                    oss << "  Low Ask \t\t: " << OrderBook::getLowPrice(asks) << "\n";
                    oss << "  Avg Ask Price \t: " << OrderBook::getAvgPrice(asks) << "\n";
                    oss << "  Total Ask Volume \t: " << OrderBook::getTotalVolume(asks);

                    response = oss.str();
                }
                else{
                    response = "ERR Invalid STATS command. Usage: STATS <PRODUCT>";
                }
            }
            else if (command == "EXIT") {
                break;
            }
            else {
                response = "ERR Unknown command. Type HELP for list.";
            }
            
            response += "\n";
            asio::write(*clientSocket, asio::buffer(response)); // Send response back to client
        }
    } catch (std::exception& e) {
        std::cerr << "Exception in handleClient: " << e.what() << std::endl;
    }
}

void ServerMain::startMatching() {
    std::vector<std::string> products = orderBook.getKnownProducts();
    std::string currentTimestamp = getCurrentTimestamp(); 
    std::cout << "Current timestamp: " << currentTimestamp << std::endl;
    if (products.empty()) {
        std::cout << "No orders available yet." << std::endl;
    }else {
        std::cout << "Initial products : "<< std::endl;
        for (const std::string& product : products) {
            std::cout << "  " << product << std::endl;
        }
    }

    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(10)); // Match every 10 seconds

        if (!isRunning) break;
        currentTimestamp = getCurrentTimestamp();
        products = orderBook.getKnownProducts();
        int executedMatches = 0;

        for (const std::string& product : products) {
            std::vector<OrderBookEntry> matchedSales = orderBook.matchAsksToBids(product, currentTimestamp, userStore);
            executedMatches += matchedSales.size();
            // for (OrderBookEntry& sale : matchedSales) {
            //     std::cout << "Sale: " << sale.product << " Price: " << sale.price << " Amount: " << sale.amount << std::endl;
            // }
        }
        std::cout << "Matching engine executed " << executedMatches << " matches at " << currentTimestamp << std::endl;
    }
}

std::string ServerMain::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::string ts = std::ctime(&now);
    ts.pop_back();
    return ts;
}
