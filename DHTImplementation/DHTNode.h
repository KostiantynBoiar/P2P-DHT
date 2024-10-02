#ifndef DHTNODE_H
#define DHTNODE_H

#include <string>

class DHTNode {
public:
    DHTNode(); // Default constructor
    DHTNode(const std::string& id); // Constructor to initialize the node with an ID

    std::string getID() const; // Returns the unique ID of the node

    static std::string generateID(); // Static method to generate a unique node ID

private:
    std::string nodeID; // Holds the unique identifier of the node
};

#endif // DHTNODE_H
