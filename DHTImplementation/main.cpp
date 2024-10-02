#include "P2P.h"
#include <iostream>
#include <thread>

int main() {
    P2P peer;

    int port;
    std::cout << "Enter the port for this peer to listen on: ";
    std::cin >> port;

    DHTNode node(DHTNode::generateID());
    peer.addNodeToDHT(node.getID());

    // Start listening for connections
    std::thread listenThread(&P2P::listenForConnections, &peer, port);

    // Start broadcasting presence
    std::thread broadcastThread(&P2P::broadcastPresence, &peer, port);

    // Start listening for peer broadcasts
    std::thread broadcastListenThread(&P2P::listenForBroadcasts, &peer);

    // Send a message to all peers
    std::string message;
    while (true) {
        std::cout << "Enter message to send to all peers: ";
        std::getline(std::cin, message);
        peer.sendMessageToAll(message);
    }

    listenThread.join();
    broadcastThread.join();
    broadcastListenThread.join();

    return 0;
}
