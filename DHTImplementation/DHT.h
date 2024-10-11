#ifndef DHT_H
#define DHT_H

#include <string>
#include <map>
#include <vector>
#include <boost/asio.hpp>
#include <openssl/sha.h> // For hashing

class DHT {
public:
    // Constructor
    DHT(const std::string& local_ip, short local_port);

    // Add a peer to the DHT
    void add_peer(const std::string& ip, short port);

    // Get all known peers
    std::map<std::string, short> get_all_peers() const;

    // Generate a unique ID for a peer using SHA256
    std::string generate_node_id(const std::string& ip, short port);

    int get_peer_count() const;

private:
    // Map to store peer IPs and ports
    std::map<std::string, short> routing_table;

    // Local node ID
    std::string local_node_id;
};


#endif // DHT_H
