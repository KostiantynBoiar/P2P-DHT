#ifndef KADEMLIA_DHT_H
#define KADEMLIA_DHT_H

#include <string>
#include <vector>
#include <unordered_map>

// Node structure
struct Node {
    std::string id;
    std::string address;

    Node(const std::string& nodeId, const std::string& nodeAddress);
};

// Kademlia DHT class
class KademliaDHT {
public:
    // Constructor
    KademliaDHT(const std::string& nodeId);

    // Store a key-value pair in the DHT
    void store(const std::string& key, const std::string& value);

    // Retrieve a value from the DHT
    std::string find_value(const std::string& key);

    // Find the closest node to the given key
    std::string find_node(const std::string& key);

    // Add a node to the routing table
    void add_node(const std::string& nodeId, const std::string& address);

private:
    // Local node ID
    std::string node_id;

    // Routing table: a list of known nodes
    std::vector<Node> routing_table;

    // Local key-value storage
    std::unordered_map<std::string, std::string> storage;
};

// Helper function to compute XOR distance between two IDs
unsigned int xor_distance(const std::string& id1, const std::string& id2);

// Helper function to generate a random node ID (useful for testing)
std::string generate_random_id();

#endif // KADEMLIA_DHT_H
