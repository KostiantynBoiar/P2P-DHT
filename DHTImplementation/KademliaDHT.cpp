#include "KademliaDHT.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

// Node constructor
Node::Node(const std::string& nodeId, const std::string& nodeAddress)
    : id(nodeId), address(nodeAddress) {}

// KademliaDHT constructor
KademliaDHT::KademliaDHT(const std::string& nodeId)
    : node_id(nodeId) {}

// Store a key-value pair in the DHT
void KademliaDHT::store(const std::string& key, const std::string& value) {
    storage[key] = value;
}

// Retrieve a value from the DHT
std::string KademliaDHT::find_value(const std::string& key) {
    if (storage.find(key) != storage.end()) {
        return storage[key];
    }
    else {
        std::string value = find_node(key);
        if (!value.empty()) {
            return value;
        }
        return "Value not found";
    }
}

// Find the closest node to the given key
std::string KademliaDHT::find_node(const std::string& key) {
    std::sort(routing_table.begin(), routing_table.end(),
        [&](const Node& a, const Node& b) {
            return xor_distance(a.id, key) < xor_distance(b.id, key);
        });

    if (!routing_table.empty()) {
        return routing_table.front().address;
    }
    else {
        return {};
    }
}

// Add a node to the routing table
void KademliaDHT::add_node(const std::string& nodeId, const std::string& address) {
    Node node(nodeId, address);
    routing_table.push_back(node);
}

// Helper function to compute XOR distance between two IDs
unsigned int xor_distance(const std::string& id1, const std::string& id2) {
    unsigned int distance = 0;
    for (size_t i = 0; i < std::min(id1.size(), id2.size()); ++i) {
        distance = (distance << 8) | (id1[i] ^ id2[i]);
    }
    return distance;
}

// Helper function to generate a random node ID
std::string generate_random_id() {
    std::string id;
    std::default_random_engine engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, 255);

    for (int i = 0; i < 20; ++i) {  // 160-bit ID (20 bytes)
        id += static_cast<char>(dist(engine));
    }
    return id;
}
