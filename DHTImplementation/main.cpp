#include "P2P.h"
#include <iostream>
#include <thread>

// Function to simulate a server that accepts incoming connections
void runServer(P2P& p2p, int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket: " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in serverAddr = p2p.createSocketAddress("0.0.0.0", port);
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        return;
    }

    std::cout << "Server listening on port " << port << std::endl;

    // Accept incoming connections in a loop
    while (true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            return;
        }
        p2p.handleNewSocket(clientSocket);
    }
}

int main() {
    P2P p2p;

    // Start the server in a separate thread
    std::thread serverThread(runServer, std::ref(p2p), 54000);

    // Connect to the server from the same machine (localhost)
    std::this_thread::sleep_for(std::chrono::seconds(1));  // Give the server time to start
    p2p.connect("127.0.0.1", 54000);

    // Send a message to the server
    p2p.send("127.0.0.1", "Hello, Server!");

    // Broadcast a message (useful if multiple connections are established)
    p2p.broadcast("Broadcasting a message to all nodes!");

    // Wait for server thread to finish (in a real application, you'd likely have more control)
    serverThread.join();

    return 0;
}
