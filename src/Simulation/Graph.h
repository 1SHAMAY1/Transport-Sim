#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <memory>

// Forward declarations
struct Edge;
struct Node;

enum class TrafficLightState {
    RED,
    YELLOW,
    GREEN,
    OFF
};

// Node represents an intersection in the road network
struct Node {
    int id;
    glm::vec3 position;  // 3D position
    std::vector<std::shared_ptr<Edge>> edges;  // Outgoing edges
    
    // Traffic Light Data (Per-Path)
    // Key: Neighbor ID (where the car is coming FROM), Value: Light State
    std::unordered_map<int, TrafficLightState> incomingLights;
    
    int currentGreenNodeId = -1; // ID of the neighbor that currently has GREEN
    float lightTimer = 0.0f;
    float minGreenDuration = 3.0f;
    float maxGreenDuration = 10.0f;
    
    Node(int id, const glm::vec3& pos) : id(id), position(pos) {}
};

// Edge represents a road connecting two nodes
struct Edge {
    std::shared_ptr<Node> from;
    std::shared_ptr<Node> to;
    float weight;  // Can represent distance, traffic, etc.
    
    Edge(std::shared_ptr<Node> f, std::shared_ptr<Node> t, float w)
        : from(f), to(t), weight(w) {}
};

// Graph Data Structure - Adjacency List representation
class Graph {
public:
    Graph() = default;
    ~Graph() = default;
    
    // Add a node to the graph
    int AddNode(const glm::vec3& position);
    
    // Add a directed edge from node 'from' to node 'to' with given weight
    void AddEdge(int fromId, int toId, float weight);
    
    // Add a bidirectional edge (creates two directed edges)
    void AddBidirectionalEdge(int fromId, int toId, float weight);
    
    // Get node by ID
    std::shared_ptr<Node> GetNode(int id);
    
    // Get all nodes
    const std::unordered_map<int, std::shared_ptr<Node>>& GetNodes() const { return m_Nodes; }
    
    // Get number of nodes
    size_t GetNodeCount() const { return m_Nodes.size(); }
    
private:
    std::unordered_map<int, std::shared_ptr<Node>> m_Nodes;
    int m_NextNodeId = 0;
};
