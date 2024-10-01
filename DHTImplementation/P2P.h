// P2P.h
#ifndef P2P_H
#define P2P_H

#include <string>
#include <map>
#include <vector>
#include <winsock2.h>   // For Windows sockets
#include <Ws2tcpip.h>   // For inet_pton

#pragma comment(lib, "Ws2_32.lib")  // Link with the Ws2_32.lib library

class P2P {
public:
    P2P(); // Constructor to initialize the P2P class

    ~P2P(); // Destructor to clean up Winsock

    void handleNewSocket(int socketFD); // Handles all incoming and outgoing TCP connections

    void send(const std::string& nodeID, const std::string& message); // Sends a message to the specified node by its ID

    void connect(const std::string& address, int port); // Connects to the specified node (address and port)

    void close(); // Closes all open connections of the client

    void broadcast(const std::string& message); // Sends packets to all connected nodes
    sockaddr_in createSocketAddress(const std::string& address, int port); // Helper function to create a socket address

private:
    int serverSocketFD; // Server socket file descriptor
    std::map<std::string, int> nodeSockets; // Map to store node ID and their socket file descriptors
    std::vector<int> connectedSockets; // List of connected socket file descriptors

};

#endif // P2P_H
