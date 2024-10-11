#ifndef P2P_H
#define P2P_H

#include <boost/asio.hpp>
#include <string>
#include "KademliaDHT.h"

class P2P {
public:
    // Constructor that takes the local port, peer IP, peer port, and reference to Kademlia DHT
    P2P(short local_port, const std::string& peer_ip, short peer_port, KademliaDHT& dht);

    // Start both server and client
    void start();

private:
    // Methods for server and client functionality
    void run_server();
    void run_client();

    // Method to handle incoming DHT commands
    void handle_message(const std::string& message, boost::asio::ip::tcp::socket& socket);

    // Server and client networking objects
    boost::asio::io_context io_context;
    short local_port;
    std::string peer_ip;
    short peer_port;

    // Reference to the DHT instance
    KademliaDHT& dht;
};

#endif // P2P_H
