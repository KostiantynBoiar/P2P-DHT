#include "P2P.h"
#include <iostream>
#include <thread>
#include <chrono>

using boost::asio::ip::tcp;

P2P::P2P(short local_port, const std::string& peer_ip, short peer_port)
    : local_port(local_port), peer_ip(peer_ip), peer_port(peer_port) {}

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
                        break; // Connection closed by peer
                    }
                    else if (error) {
                        throw boost::system::system_error(error); // Some other error
                    }

                    std::cout << "Received: " << std::string(data, length) << std::endl;
                    boost::asio::write(socket, boost::asio::buffer("Message received\n"), error);
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

        // Keep attempting to connect until successful
        while (true) {
            auto endpoints = resolver.resolve(peer_ip, std::to_string(peer_port), ec);
            if (!ec) {
                boost::asio::connect(socket, endpoints, ec);
                if (!ec) {
                    std::cout << "Connected to peer " << peer_ip << ":" << peer_port << std::endl;
                    break; // Successfully connected, exit the retry loop
                }
                else {
                    std::cerr << "Connection failed: " << ec.message() << ". Retrying in 5 seconds..." << std::endl;
                }
            }
            else {
                std::cerr << "Resolution failed: " << ec.message() << ". Retrying in 5 seconds..." << std::endl;
            }

            // Wait for 5 seconds before retrying
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }

        while (true) {
            std::cout << "Enter message to send (type 'exit' to quit): ";
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

void P2P::start() {
    // Run server and client on separate threads
    std::thread server_thread([this]() {
        run_server();
        });

    std::thread client_thread([this]() {
        run_client();
        });

    // Join the threads to allow both to work concurrently
    server_thread.join();
    client_thread.join();
}
