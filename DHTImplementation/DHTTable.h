// DHTTable.h
#ifndef DHTTABLE_H
#define DHTTABLE_H

#include <map>
#include <string>
#include "DHTNode.h"

class DHTTable {
public:
    DHTTable(); // Constructor to initialize the DHT table

    void addNode(const DHTNode& node); // Adds a node to the DHT table

    DHTNode neighbor(const std::string& targetID) const; // Finds and returns the nearest node to the targetID

private:
    std::map<std::string, DHTNode> nodeMap; // Map to store node IDs and their corresponding DHTNode objects
};

#endif // DHTTABLE_H
