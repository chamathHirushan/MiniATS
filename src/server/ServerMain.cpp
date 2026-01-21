#include "ServerMain.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <ctime>
#include <csignal>
#include "CSVHandler.hpp"
#include "CrowHelper.hpp"

ServerMain* ServerMain::serverInstance = nullptr; // Global pointer to current instance

void ServerMain::cleanup(int signum) {
    std::cout << "\nShutting down server..." << std::endl;
    if (serverInstance != nullptr) {
        
        CSVHandler::entriesToCSV("sales.csv", serverInstance->orderBook.getSales());
        CSVHandler::entriesToCSV(serverInstance->orderBook.getFilename(), serverInstance->orderBook.getAllOrders(), false);
        
        serverInstance->userStore.save();
        //serverInstance->orderBook.save();
        
        std::cout << "Exported records successfully." << std::endl;

        serverInstance->isRunning = false;
        serverInstance->matchingCV.notify_all();
        for (auto& f : serverInstance->matchingFutures) { // Wait for all matching threads to finish their work
            if (f.valid()) {
                f.wait();
            }
        }
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
        
        // Load data from persistence files
        userStore.load();
        //orderBook.load();
        
    } catch (std::exception& e) {
        std::cerr << "Exception in init: " << e.what() << std::endl;
        return;
    }

    // starting the matching engine in a separate thread
    std::thread matchingEngine(&ServerMain::startMatching, this);
    matchingEngine.detach();

    // Start REST API in a separate thread
    std::cout << "Starting REST API..." << std::endl;
    startRestServer();

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
                        
                        startMatchingProduct(product); // Trigger matching for the product

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
                    std::vector<OrderBookEntry> orders = orderBook.getOrdersForUser(username);
                    int count = 0;
                    oss << "Ongoing orders of user " << username << ":\n";
                    for (const auto& order : orders) {
                        count++;
                        oss << " ID: " << order.id
                            << " | " << OrderBookEntry::orderTypeToString(order.orderType)
                            << " | " << order.product
                            << " | Amount: " << order.amount
                            << " | Price: " << order.price
                            << " | Timestamp: " << order.timestamp << "\n";
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
                        std::vector<OrderBookEntry> orders = orderBook.getOrdersForUser(username);
                        bool found = false;
                        for (auto& order : orders) {
                            if (orderId == order.id) {
                                found = true;
                                userStore.getUser(username).getWallet().cancelOrder(order);
                                break;
                            }
                        }
                        if (found) {
                            bool success= orderBook.removeOrderById(username, orderId);
                            if (success)
                                response = "OK Order cancelled.";
                            else
                                response = "ERR Order not found.";
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
                    std::unordered_map<OrderBookType, std::vector<OrderBookEntry*>> productOrders = orderBook.getOrders(product);
                    if (!productOrders.empty()) {
                        oss << "\n Product: " << product << "\t Best bid: "
                            << OrderBook::getHighPrice(productOrders[OrderBookType::bid])
                            << "\t Best ask: "
                            << OrderBook::getLowPrice(productOrders[OrderBookType::ask]);
                    }
                }
                response = oss.str();
            }
            else if (command == "STATS") {
                if (tokens.size() == 2) {
                    std::string product = tokens[1];
                    std::transform(product.begin(), product.end(), product.begin(),
                       [](unsigned char c){ return std::toupper(c); });
                    std::ostringstream oss;

                    std::unordered_map<OrderBookType, std::vector<OrderBookEntry*>> productOrders = orderBook.getOrders(product);

                    // ----- BIDS -----
                    std::vector<OrderBookEntry*> bids = productOrders[OrderBookType::bid];
                    oss << "\n  Bids \t\t\t: " << bids.size() << "\n";
                    oss << "  High Bid \t\t: " << OrderBook::getHighPrice(bids) << "\n";
                    oss << "  Avg Bid Price \t: " << OrderBook::getAvgPrice(bids) << "\n";
                    oss << "  Total Bid Volume \t: " << OrderBook::getTotalVolume(bids) << "\n";

                    // ----- ASKS -----
                    std::vector<OrderBookEntry*> asks = productOrders[OrderBookType::ask];
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
    
    {
        std::lock_guard<std::mutex> lock(matchingMutex);
        for (const auto& p : products) {
            pendingProducts.push_back(p);
        }
    }
    
    std::cout << "Matching engine started." << std::endl;

    while (isRunning) {
        std::string product;
        {
            std::unique_lock<std::mutex> lock(matchingMutex);
            matchingCV.wait(lock, [this]{ return !pendingProducts.empty() || !isRunning; }); // wait for a product to be available

            if (!isRunning) break; // exit if server is stopping

            product = pendingProducts.front();
            pendingProducts.pop_front();
        }

        // Remove tasks that are already finished
        matchingFutures.erase(
            std::remove_if(matchingFutures.begin(), matchingFutures.end(),
                [](std::future<void>& f) {
                    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
                }),
            matchingFutures.end()
        );

        // Add the new task
        matchingFutures.push_back(std::async(std::launch::async, [this, product]() {
            std::string currentTimestamp = getCurrentTimestamp();
            std::vector<OrderBookEntry> matchedSales = orderBook.matchAsksToBids(product, currentTimestamp, userStore);
            
            if (!matchedSales.empty()) {
                std::cout << "Matching engine executed " << matchedSales.size() << " matches for " << product << " at " << currentTimestamp << std::endl;
            }
        }));
    }
}

void ServerMain::startMatchingProduct(std::string product) {
    {
        std::lock_guard<std::mutex> lock(matchingMutex);
        if (std::find(pendingProducts.begin(), pendingProducts.end(), product) == pendingProducts.end()) {
            pendingProducts.push_back(product); // add only if not present
        }
    }
    matchingCV.notify_one();
}

std::string ServerMain::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::string ts = std::ctime(&now);
    ts.pop_back();
    return ts;
}

void ServerMain::startRestServer() {
    restThread = std::thread([this]() {
        crow::App<CORS> app;

        CROW_ROUTE(app, "/health")
        ([](){
            return "OK";
        });

        CROW_ROUTE(app, "/products")
        ([this](){ // Capture this to access orderBook
            std::vector<std::string> products = this->orderBook.getKnownProducts();
            crow::json::wvalue x;
            for(size_t i = 0; i < products.size(); i++) {
                x[i] = products[i];
            }
            return x;
        });

        // Get orders for a specific product
        CROW_ROUTE(app, "/orders")
        ([this](const crow::request& req){
            auto productVal = req.url_params.get("product");
            if (!productVal) {
                return crow::response(400, "Missing 'product' parameter");
            }
            std::string product(productVal);
            
            auto ordersMap = this->orderBook.getOrders(product);
            
            crow::json::wvalue res;
            int idx = 0;
            
            auto addOrders = [&](const std::vector<OrderBookEntry*>& orders) {
                for (const auto* order : orders) {
                    res[idx]["id"] = order->id;
                    res[idx]["price"] = order->price;
                    res[idx]["amount"] = order->amount;
                    res[idx]["timestamp"] = order->timestamp;
                    res[idx]["product"] = order->product;
                    res[idx]["type"] = OrderBookEntry::orderTypeToString(order->orderType);
                    res[idx]["username"] = order->username;
                    idx++;
                }
            };

            if (ordersMap.count(OrderBookType::ask)) addOrders(ordersMap[OrderBookType::ask]);
            if (ordersMap.count(OrderBookType::bid)) addOrders(ordersMap[OrderBookType::bid]);

            return crow::response(res);
        });

        // Place an order
        CROW_ROUTE(app, "/order").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req){
            auto x = crow::json::load(req.body);
            if (!x) {
                return crow::response(400, "Invalid JSON");
            }

            try {
                std::string product = x["product"].s();
                double price = x["price"].d();
                double amount = x["amount"].d();
                std::string typeStr = x["type"].s();
                std::string username = x["username"].s();

                OrderBookType type = OrderBookEntry::determineOrderType(typeStr);
                
                // Use getCurrentTimestamp() from ServerMain
                OrderBookEntry entry(price, amount, this->getCurrentTimestamp(), product, type, username); 
                
                // Check wallet funds
                if (!this->userStore.getUser(username).getWallet().fulfillOrder(entry)) {
                   return crow::response(400, "Insufficient funds");
                }

                this->orderBook.insertOrder(entry);
                this->startMatchingProduct(product);
                
                return crow::response(201, "Order created");
            } catch (const std::exception& e) {
                return crow::response(400, "Invalid order data");
            }
        });

        // Register
        CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req){
            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400, "Invalid JSON");
            
            try {
                std::string username = x["username"].s();
                std::string password = x["password"].s();

                if (this->userStore.userExists(username)) {
                    return crow::response(400, "User already exists");
                }
                
                this->userStore.addUser(username, password);
                this->userStore.save();
                return crow::response(201, "User registered");
            } catch (...) {
                return crow::response(500, "Internal error");
            }
        });

        // Login
        CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req){
            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400, "Invalid JSON");

            try {
                std::string username = x["username"].s();
                std::string password = x["password"].s();

                if (!this->userStore.userExists(username)) {
                    return crow::response(401, "User not found");
                }
                
                User& user = this->userStore.getUser(username);
                if (user.validatePassword(password)) {
                     return crow::response(200, "OK");
                } else {
                     return crow::response(401, "Invalid password");
                }
            } catch (...) {
                return crow::response(500, "Internal error");
            }
        });

        // Get Wallet
        CROW_ROUTE(app, "/wallet")
        ([this](const crow::request& req){
            auto u = req.url_params.get("username");
            if (!u) return crow::response(400, "Missing username");
            std::string username(u);

            if (!this->userStore.userExists(username)) {
                return crow::response(404, "User not found");
            }

            User& user = this->userStore.getUser(username);
            nlohmann::json j;
            to_json(j, user.getWallet());
            
            return crow::response(j.dump());
        });

        // Wallet Deposit
        CROW_ROUTE(app, "/wallet/deposit").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req){
            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400, "Invalid JSON");
            
            try {
                std::string username = x["username"].s();
                std::string product = x["product"].s();
                double amount = x["amount"].d();
                
                if (!this->userStore.userExists(username)) return crow::response(404, "User not found");

                this->userStore.getUser(username).getWallet().insertCurrency(product, amount);
                this->userStore.save(); // Check if save is thread-safe or needed
                
                return crow::response(200, "Deposit successful");
            } catch (...) {
                return crow::response(400, "Invalid data");
            }
        });

        // Wallet Withdraw
        CROW_ROUTE(app, "/wallet/withdraw").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req){
            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400, "Invalid JSON");
            
            try {
                std::string username = x["username"].s();
                std::string product = x["product"].s();
                double amount = x["amount"].d();
                
                if (!this->userStore.userExists(username)) return crow::response(404, "User not found");

                if (this->userStore.getUser(username).getWallet().removeCurrency(product, amount)) {
                    this->userStore.save();
                    return crow::response(200, "Withdraw successful");
                } else {
                    return crow::response(400, "Insufficient funds");
                }
            } catch (...) {
                return crow::response(400, "Invalid data");
            }
        });

        // Get User Orders
        CROW_ROUTE(app, "/user/orders")
        ([this](const crow::request& req){
            auto u = req.url_params.get("username");
            if (!u) return crow::response(400, "Missing username");
            std::string username(u);

            std::vector<OrderBookEntry> orders = this->orderBook.getOrdersForUser(username);
            
            std::vector<crow::json::wvalue> jsonOrders;
            for (const auto& order : orders) {
                crow::json::wvalue j;
                j["id"] = order.id;
                j["price"] = order.price;
                j["amount"] = order.amount;
                j["timestamp"] = order.timestamp;
                j["product"] = order.product;
                j["type"] = OrderBookEntry::orderTypeToString(order.orderType);
                j["username"] = order.username;
                jsonOrders.push_back(j);
            }
            crow::json::wvalue res = jsonOrders;
            return crow::response(res);
        });

        // Cancel Order
        CROW_ROUTE(app, "/order").methods(crow::HTTPMethod::DELETE)
        ([this](const crow::request& req){
            auto idVal = req.url_params.get("id");
            auto userVal = req.url_params.get("username");
            
            if (!idVal || !userVal) return crow::response(400, "Missing id or username");
            
            int id = std::stoi(idVal);
            std::string username(userVal);

            // Find order to cancel (need to update wallet logic: return funds)
            // Need to verify ownership and existing logic in ServerHandleClient
            
            std::vector<OrderBookEntry> orders = this->orderBook.getOrdersForUser(username);
            bool found = false;
            OrderBookEntry foundOrder(0,0,"","",OrderBookType::ask, "");

            for (auto& order : orders) {
                if (id == order.id) {
                    found = true;
                    foundOrder = order;
                    break;
                }
            }

            if (found) {
                // Return funds
                this->userStore.getUser(username).getWallet().cancelOrder(foundOrder);
                
                // Remove from OrderBook
                bool success = this->orderBook.removeOrderById(username, id);
                if (success) {
                    return crow::response(200, "Order cancelled");
                } else {
                   return crow::response(404, "Order not found in book");
                }
            }
             
            return crow::response(404, "Order not found");
        });

        // app.loglevel(crow::LogLevel::Warning);
        app.signal_clear(); // Don't handle signals, let the main server handle them
        app.port(18080).multithreaded().run();
    });
    
    restThread.detach();
}

