#include "ClientMain.hpp"
#include <iostream>
#include <string>

using asio::ip::tcp; // take TCP protocol from asio::ip namespace

void ClientMain::init() {
    std::cout << "ClientMain starting..." << std::endl;
    run();
}

void ClientMain::run() {
    try {
        // Resolve server address and port
        tcp::resolver resolver{io_context};
        auto endpoints = resolver.resolve("localhost", "5322");
        
        // Create socket and connect to server
        tcp::socket socket{io_context};
        asio::connect(socket, endpoints);

        std::cout << "Connected to server!" << std::endl;

        char recvbuf[512]; // Buffer to hold received data
        asio::error_code error; // To capture any asio errors

        // Receive welcome message from server
        size_t len = socket.read_some(asio::buffer(recvbuf), error); // ASIO read_some read the data that came from socket, and put into buffer. returns number of bytes read.

        if (!error) {
            std::cout << "Server: " << std::string(recvbuf, len) << std::endl; // display only the top len number of bytes from buffer
        }

        // Client input loop
        std::string userInput; // buffer to hold user input
        while(true) {
            std::cout << "Type HELP for commands" << std::endl;
            std::cout << "> ";
            std::getline(std::cin, userInput);

            if (userInput == "EXIT" || userInput == "exit") {
                break;
            }

            if (userInput == "HELP" || userInput == "help") {
                std::cout << "  Register to the platform: REGISTER <user> <password>"<<std::endl;
                std::cout << "  Login to the platform:    LOGIN <user> <password>"<<std::endl;
                std::cout << "  Place an order:           ASK/BID <prod> <amount> <price>"<<std::endl;
                std::cout << "  View wallet:              WALLET"<<std::endl;
                std::cout << "  Deposit to wallet:        DEPOSIT <prod> <amount>"<<std::endl;
                std::cout << "  Withdraw from wallet:     WITHDRAW <prod> <amount>"<<std::endl;
                std::cout << "  View market:              MARKET"<<std::endl;
                std::cout << "  Exit the application:     EXIT" << std::endl;
                std::cout << std::endl;
                continue;
            }

            if (userInput.length() > 0) {
                // Send user input to server
                asio::write(socket, asio::buffer(userInput)); // ASIO write sends data through the socket

                // Read server response
                len = socket.read_some(asio::buffer(recvbuf), error); // Read any available data
                
                if (error == asio::error::eof) { // Connection closed by server
                    std::cout << "Connection closed" << std::endl;
                    break;
                } else if (error) { // Other errors
                    throw asio::system_error(error);
                }

                std::cout << "  Server: " << std::string(recvbuf, len) << std::endl;
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}