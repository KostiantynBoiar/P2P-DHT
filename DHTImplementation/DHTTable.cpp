#include "DHTTable.h"
#include <limits>
#include <algorithm>
#include <stdexcept>

// Constructor
DHTTable::DHTTable() {}

// Adds a node to the DHT table
void DHTTable::addNode(const DHTNode& node) {
    nodeMap[node.getID()] = node;
}

// Finds and returns the nearest node to the targetID
DHTNode DHTTable::neighbor(const std::string& targetID) const {
    if (nodeMap.empty()) {
        throw std::runtime_error("DHT table is empty.");
    }

    // Find the closest node based on ID
    auto closestNode = std::min_element(nodeMap.begin(), nodeMap.end(),
        [&](const std::pair<std::string, DHTNode>& a, const std::pair<std::string, DHTNode>& b) {
            return std::abs(std::stoll(a.first, nullptr, 16) - std::stoll(targetID, nullptr, 16)) <
                std::abs(std::stoll(b.first, nullptr, 16) - std::stoll(targetID, nullptr, 16));
        });

    return closestNode->second;
}
