#ifndef P2P_H
#define P2P_H

#include <string>
#include <map>
#include <set>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "DHTTable.h"
#include "DHTNode.h"

#pragma comment(lib, "Ws2_32.lib")

class P2P {
public:
    P2P(); // Constructor to initialize the P2P class
    ~P2P(); // Destructor to clean up Winsock

    void broadcastPresence(int port); // Broadcast this peer's presence on the network
    void listenForBroadcasts(); // Listen for broadcast messages and connect to discovered peers
    void handleNewSocket(int socketFD); // Handle new socket connections from other peers
    void sendMessageToPeer(const std::string& peerID, const std::string& message); // Send a message to a specific peer
    void sendMessageToAll(const std::string& message); // Send a message to all connected peers
    void listenForConnections(int port); // Listen for incoming peer connections

    void connect(const std::string& address, int port); // Connect to another peer
    void close(); // Close all connections

    void addNodeToDHT(const std::string& id); // Adds a node to the DHT
    DHTNode findClosestNode(const std::string& targetID); // Finds the closest node in the DHT

    int serverSocketFD; // Server socket file descriptor
    std::map<std::string, int> nodeSockets; // Map to store node ID and their socket file descriptors
    std::set<std::string> connectedPeers; // Set to track connected peers
    sockaddr_in createSocketAddress(const std::string& address, int port); // Helper function to create a socket address
    void send(const std::string& nodeID, const std::string& message); // Send a message over a specific socket
};

#endif // P2P_H
