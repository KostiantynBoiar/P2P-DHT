#include "P2P.h"
#include <iostream>

// Constructor: Initializes Winsock
P2P::P2P() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        exit(1); // Exit on error
    }
    serverSocketFD = INVALID_SOCKET;
}

// Destructor: Cleans up Winsock
P2P::~P2P() {
    close();  // Close all sockets
    WSACleanup(); // Clean up Winsock
}

// Creates a socket address for connecting or listening
sockaddr_in P2P::createSocketAddress(const std::string& address, int port) {
    sockaddr_in socketAddr;
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &socketAddr.sin_addr);
    return socketAddr;
}

// Handles a new socket, typically for the server to manage incoming connections
void P2P::handleNewSocket(int socketFD) {
    connectedSockets.push_back(socketFD);
    std::cout << "New connection accepted: Socket FD = " << socketFD << std::endl;
}

// Sends a message to a specific node using its ID
void P2P::send(const std::string& nodeID, const std::string& message) {
    auto it = nodeSockets.find(nodeID);
    if (it != nodeSockets.end()) {
        int socketFD = it->second;
        int result = ::send(socketFD, message.c_str(), message.size(), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "Failed to send message to " << nodeID << " with error: " << WSAGetLastError() << std::endl;
        }
        else {
            std::cout << "Message sent to " << nodeID << ": " << message << std::endl;
        }
    }
    else {
        std::cerr << "Node ID " << nodeID << " not found." << std::endl;
    }
}

// Connects to a remote node
void P2P::connect(const std::string& address, int port) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in serverAddr = createSocketAddress(address, port);
    int result = ::connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Connection failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return;
    }

    std::cout << "Connected to " << address << " on port " << port << std::endl;
    connectedSockets.push_back(clientSocket);

    // Store the connection in the nodeSockets map (assuming the address is used as the nodeID here)
    nodeSockets[address] = clientSocket;
}

// Closes all open connections
void P2P::close() {
    for (auto& socketFD : connectedSockets) {
        closesocket(socketFD);
    }
    connectedSockets.clear();
    nodeSockets.clear();
}

// Broadcasts a message to all connected nodes
void P2P::broadcast(const std::string& message) {
    for (int socketFD : connectedSockets) {
        int result = ::send(socketFD, message.c_str(), message.size(), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "Broadcast failed for socket FD " << socketFD << " with error: " << WSAGetLastError() << std::endl;
        }
        else {
            std::cout << "Message broadcasted to socket FD " << socketFD << ": " << message << std::endl;
        }
    }
}
