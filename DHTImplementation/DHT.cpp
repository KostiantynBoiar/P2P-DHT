#include "DHT.h"
#include <iostream>
#include <openssl/sha.h> // For hashing
#include <iomanip>

DHT::DHT(const std::string& local_ip, short local_port) {
    local_node_id = generate_node_id(local_ip, local_port);
    std::cout << "Local node ID: " << local_node_id << std::endl;
}

void DHT::add_peer(const std::string& ip, short port) {
    std::string node_id = generate_node_id(ip, port);

    // Check if the peer already exists in the routing table
    if (routing_table.find(ip) == routing_table.end()) {
        routing_table[ip] = port;
        std::cout << "Added peer: " << ip << ":" << port << " with node ID: " << node_id << std::endl;
    }
    else {
        std::cout << "Peer already exists: " << ip << ":" << port << std::endl;
    }
}

std::map<std::string, short> DHT::get_all_peers() const {
    return routing_table;
}

std::string DHT::generate_node_id(const std::string& ip, short port) {
    std::string input = ip + ":" + std::to_string(port);
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

int DHT::get_peer_count() const {
    return routing_table.size();
}