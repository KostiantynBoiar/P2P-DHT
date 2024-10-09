#include "P2P.h"
#include <iostream>

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
