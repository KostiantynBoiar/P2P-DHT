#include "P2P.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <thread>
#include <chrono>
#include <set>

// Constants
const int BROADCAST_PORT = 55000;
const int BROADCAST_INTERVAL = 5000; // Broadcast every 5 seconds

P2P::P2P() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed.");
    }

    // Create a server socket
    serverSocketFD = INVALID_SOCKET;
}

P2P::~P2P() {
    close();
    WSACleanup();
}

// Helper function to create a socket address
sockaddr_in P2P::createSocketAddress(const std::string& address, int port) {
    sockaddr_in socketAddress;
    memset(&socketAddress, 0, sizeof(socketAddress));

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, address.c_str(), &socketAddress.sin_addr) <= 0) {
        throw std::runtime_error("Invalid IP address format.");
    }

    return socketAddress;
}

// Add a node to the DHT
void P2P::addNodeToDHT(const std::string& id) {
    DHTNode node(id);
    std::cout << "Node added to DHT: " << id << std::endl;
}

// Send a message to a specific peer
void P2P::sendMessageToPeer(const std::string& peerID, const std::string& message) {
    send(peerID, message);
}

// Send a message to all connected peers
void P2P::sendMessageToAll(const std::string& message) {
    for (const auto& [nodeID, socketFD] : nodeSockets) {
        send(nodeID, message);
    }
}

// Internal function to send a message to a specific node
void P2P::send(const std::string& nodeID, const std::string& message) {
    auto it = nodeSockets.find(nodeID);
    if (it == nodeSockets.end()) {
        std::cerr << "Node ID not found!" << std::endl;
        return;
    }

    int socketFD = it->second;
    int result = ::send(socketFD, message.c_str(), message.length(), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Failed to send message to node: " << nodeID << std::endl;
    }
    else {
        std::cout << "Message sent to node: " << nodeID << std::endl;
    }
}

// Broadcast this peer's presence
void P2P::broadcastPresence(int port) {
    SOCKET broadcastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (broadcastSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create broadcast socket: " << WSAGetLastError() << std::endl;
        return;
    }

    BOOL broadcastOption = TRUE;
    
    if (setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastOption, sizeof(broadcastOption)) < 0) {
        std::cerr << "Failed to set broadcast option: " << WSAGetLastError() << std::endl;
        closesocket(broadcastSocket);
        return;
    }

    sockaddr_in broadcastAddr;
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(BROADCAST_PORT);
    broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

    std::string message = "Presence: " + std::to_string(port);

    while (true) {
        sendto(broadcastSocket, message.c_str(), message.size(), 0, (sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
        std::this_thread::sleep_for(std::chrono::milliseconds(BROADCAST_INTERVAL));
    }

    closesocket(broadcastSocket);
}

// Listen for broadcast messages and connect to discovered peers
void P2P::listenForBroadcasts() {
    SOCKET listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create UDP listen socket: " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in listenAddr;
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(BROADCAST_PORT);
    listenAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, (sockaddr*)&listenAddr, sizeof(listenAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        return;
    }

    char buffer[512];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    while (true) {
        int bytesReceived = recvfrom(listenSocket, buffer, 512, 0, (sockaddr*)&senderAddr, &senderAddrSize);
        if (bytesReceived > 0) {
            std::string message(buffer, 0, bytesReceived);
            std::size_t colonPos = message.find(":");
            if (colonPos != std::string::npos) {
                std::string peerIP = message.substr(0, colonPos);
                int peerPort = std::stoi(message.substr(colonPos + 1));

                std::string peerID = peerIP + ":" + std::to_string(peerPort);

                if (connectedPeers.find(peerID) == connectedPeers.end()) {
                    std::cout << "Discovered peer at " << peerID << std::endl;
                    connect(peerIP, peerPort);
                    connectedPeers.insert(peerID);
                }
            }
        }
    }

    closesocket(listenSocket);
}

// Handle new socket connections
void P2P::handleNewSocket(int socketFD) {
    connectedPeers.insert(std::to_string(socketFD)); // Simulated peer ID
    std::cout << "New socket connection established: " << socketFD << std::endl;
    nodeSockets[std::to_string(socketFD)] = socketFD;
}

void P2P::listenForConnections(int port) {
    // Initialize Winsock if it hasn't been initialized yet
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return;
    }

    // Create the server socket if it hasn't been initialized
    serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFD == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket: " << WSAGetLastError() << std::endl;
        return;
    }

    // Confirm that the socket is valid
    if (serverSocketFD == INVALID_SOCKET) {
        std::cerr << "Socket is invalid after creation: " << WSAGetLastError() << std::endl;
        return;
    }
    else {
        std::cout << "Socket successfully created: " << serverSocketFD << std::endl;
    }

    sockaddr_in peerAddress = createSocketAddress("0.0.0.0", port);

    // Bind the socket to the port
    if (bind(serverSocketFD, (sockaddr*)&peerAddress, sizeof(peerAddress)) == SOCKET_ERROR) {
        std::cerr << "Peer: Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocketFD);
        serverSocketFD = INVALID_SOCKET;
        return;
    }
    else {
        std::cout << "Socket successfully bound to port " << port << std::endl;
    }

    // Listen for incoming connections
    if (listen(serverSocketFD, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Peer: Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocketFD);
        serverSocketFD = INVALID_SOCKET;
        return;
    }

    std::cout << "Peer listening on port " << port << "..." << std::endl;

    while (true) {
        sockaddr_in peerClientAddress;
        int peerClientAddressSize = sizeof(peerClientAddress);
        int peerSocket = accept(serverSocketFD, (sockaddr*)&peerClientAddress, &peerClientAddressSize);
        if (peerSocket == INVALID_SOCKET) {
            std::cerr << "Peer: Accept failed with error: " << WSAGetLastError() << std::endl;
            return;
        }

        std::cout << "Peer: Connected to another peer!" << std::endl;
        handleNewSocket(peerSocket);

        char buffer[512];
        int bytesReceived = recv(peerSocket, buffer, 512, 0);
        if (bytesReceived > 0) {
            std::cout << "Peer received message: " << std::string(buffer, 0, bytesReceived) << std::endl;
        }
    }
}
// Connect to another peer
void P2P::connect(const std::string& address, int port) {
    sockaddr_in targetAddress = createSocketAddress(address, port);

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        throw std::runtime_error("Failed to create socket for connecting.");
    }

    if (::connect(clientSocket, (sockaddr*)&targetAddress, sizeof(targetAddress)) == SOCKET_ERROR) {
        closesocket(clientSocket);
        throw std::runtime_error("Failed to connect to node.");
    }

    std::string nodeID = address + ":" + std::to_string(port);
    nodeSockets[nodeID] = clientSocket;
    std::cout << "Connected to node: " << nodeID << std::endl;

    addNodeToDHT(nodeID);
}

// Close all connections
void P2P::close() {
    for (auto& [nodeID, socketFD] : nodeSockets) {
        closesocket(socketFD);
    }
    nodeSockets.clear();

    if (serverSocketFD != INVALID_SOCKET) {
        closesocket(serverSocketFD);
        serverSocketFD = INVALID_SOCKET;
    }

    std::cout << "Closed all connections." << std::endl;
}
