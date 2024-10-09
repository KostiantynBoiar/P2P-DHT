#ifndef P2P_H
#define P2P_H

#include <boost/asio.hpp>
#include <string>

class P2P {
public:
    // Constructor that takes the local port, peer IP, and peer port
    P2P(short local_port, const std::string& peer_ip, short peer_port);

    // Start both server and client
    void start();

private:
    // Methods for server and client functionality
    void run_server();
    void run_client();

    // Server and client networking objects
    boost::asio::io_context io_context;
    short local_port;
    std::string peer_ip;
    short peer_port;
};

#endif // P2P_H
