#include "P2P.h"
#include <iostream>
/*
int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: p2p_peer <your_port> <peer_ip> <peer_port>" << std::endl;
        return 1;
    }

    short local_port = std::atoi(argv[1]);
    std::string peer_ip = argv[2];
    short peer_port = std::atoi(argv[3]);

    P2P p2p(local_port, peer_ip, peer_port);
    p2p.start();

    return 0;
}
*/
#include "KademliaDHT.h"
#include "P2P.h"

int main() {
    // Initialize a Kademlia DHT instance for this peer
    std::string myNodeId = generate_random_id();
    KademliaDHT dht(myNodeId);

    // Create a P2P instance and link it to the DHT
    int port1, port2;
    std::string ip;
    std::cin >> port1 >> ip >> port2;
    P2P p2p(port1, ip, port2, dht);

    // Start P2P and DHT functionalities
    p2p.start();

    return 0;
}
