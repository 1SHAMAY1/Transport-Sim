#include "Graph.h"
#include <stdexcept>

int Graph::AddNode(const glm::vec3& position) {
    int id = m_NextNodeId++;
    auto node = std::make_shared<Node>(id, position);
    m_Nodes[id] = node;
    return id;
}

void Graph::AddEdge(int fromId, int toId, float weight) {
    auto fromNode = GetNode(fromId);
    auto toNode = GetNode(toId);
    
    if (!fromNode || !toNode) {
        throw std::runtime_error("Invalid node ID in AddEdge");
    }
    
    auto edge = std::make_shared<Edge>(fromNode, toNode, weight);
    fromNode->edges.push_back(edge);
}

void Graph::AddBidirectionalEdge(int fromId, int toId, float weight) {
    AddEdge(fromId, toId, weight);
    AddEdge(toId, fromId, weight);
}

std::shared_ptr<Node> Graph::GetNode(int id) {
    auto it = m_Nodes.find(id);
    if (it != m_Nodes.end()) {
        return it->second;
    }
    return nullptr;
}
