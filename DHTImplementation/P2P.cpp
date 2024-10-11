#include "P2P.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

using boost::asio::ip::tcp;

P2P::P2P(short local_port, const std::string& peer_ip, short peer_port, KademliaDHT& dht)
    : local_port(local_port), peer_ip(peer_ip), peer_port(peer_port), dht(dht) {}

void P2P::run_server() {
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), local_port));
    std::cout << "Server is listening on port " << local_port << std::endl;

    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);
        std::cout << "New peer connected!" << std::endl;
        std::thread([this, socket = std::move(socket)]() mutable {
            try {
                boost::system::error_code error;
                while (true) {
                    char data[1024];
                    size_t length = socket.read_some(boost::asio::buffer(data), error);
                    if (error == boost::asio::error::eof) {
                        std::cout << "Client disconnected." << std::endl;
                        break;
                    }
                    else if (error) {
                        throw boost::system::system_error(error);
                    }

                    std::string message(data, length);
                    std::cout << "Received: " << message << std::endl;

                    // Handle DHT-related message
                    handle_message(message, socket);

                }
            }
            catch (std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
            }).detach();
    }
}

void P2P::run_client() {
    try {
        tcp::resolver resolver(io_context);
        tcp::socket socket(io_context);
        boost::system::error_code ec;

        std::cout << "Attempting to connect to peer " << peer_ip << ":" << peer_port << std::endl;

        while (true) {
            auto endpoints = resolver.resolve(peer_ip, std::to_string(peer_port), ec);
            if (!ec) {
                boost::asio::connect(socket, endpoints, ec);
                if (!ec) {
                    std::cout << "Connected to peer " << peer_ip << ":" << peer_port << std::endl;
                    break;
                }
                else {
                    std::cerr << "Connection failed: " << ec.message() << ". Retrying in 5 seconds..." << std::endl;
                }
            }
            else {
                std::cerr << "Resolution failed: " << ec.message() << ". Retrying in 5 seconds..." << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(5));
        }

        while (true) {
            std::cout << "Enter DHT command (e.g., 'STORE <key> <value>') or 'exit' to quit: ";
            std::string message;
            std::getline(std::cin, message);
            if (message == "exit") break;

            boost::asio::write(socket, boost::asio::buffer(message + "\n"), ec);
            if (ec) {
                std::cerr << "Failed to send message: " << ec.message() << std::endl;
                break;
            }

            char reply[1024];
            size_t reply_length = socket.read_some(boost::asio::buffer(reply), ec);
            if (ec) {
                std::cerr << "Failed to read reply: " << ec.message() << std::endl;
                break;
            }
            std::cout << "Reply from peer: " << std::string(reply, reply_length) << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void P2P::handle_message(const std::string& message, tcp::socket& socket) {
    std::istringstream iss(message);
    std::string command, key, value;
    iss >> command >> key;

    if (command == "STORE") {
        iss >> value;
        dht.store(key, value);
        std::string response = "Stored " + key + " with value " + value + "\n";
        boost::asio::write(socket, boost::asio::buffer(response));
    }
    else if (command == "FIND_VALUE") {
        std::string result = dht.find_value(key);
        std::string response = "Value for key " + key + ": " + result + "\n";
        boost::asio::write(socket, boost::asio::buffer(response));
    }
    else if (command == "FIND_NODE") {
        std::string result = dht.find_node(key);
        std::string response = "Closest node for key " + key + ": " + result + "\n";
        boost::asio::write(socket, boost::asio::buffer(response));
    }
    else {
        std::string response = "Unknown command\n";
        boost::asio::write(socket, boost::asio::buffer(response));
    }
}

void P2P::start() {
    std::thread server_thread([this]() {
        run_server();
        });

    std::thread client_thread([this]() {
        run_client();
        });

    server_thread.join();
    client_thread.join();
}
