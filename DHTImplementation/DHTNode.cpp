#include "DHTNode.h"
#include <random>
#include <sstream>

// Default constructor
DHTNode::DHTNode() : nodeID("") {}  // Initializes nodeID with an empty string

// Constructor to initialize the node with an ID
DHTNode::DHTNode(const std::string& id) : nodeID(id) {}

// Returns the unique ID of the node
std::string DHTNode::getID() const {
    return nodeID;
}

// Static method to generate a unique node ID
std::string DHTNode::generateID() {
    std::stringstream ss;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15); // Hexadecimal

    for (int i = 0; i < 16; ++i) {
        ss << std::hex << dis(gen);
    }

    return ss.str(); // Returns a 16-character hex string
}
