#include "Pathfinding.h"
#include <iostream>

float Pathfinding::Heuristic(const glm::vec3& a, const glm::vec3& b) {
    return glm::length(b - a);  // Euclidean distance
}

std::vector<int> Pathfinding::AStar(
    std::shared_ptr<Graph> graph,
    int startId,
    int goalId
) {
    // Priority queue (min-heap) - stores nodes to explore
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
    
    // Track visited nodes
    std::unordered_set<int> closedSet;
    
    // Track best gCost for each node
    std::unordered_map<int, float> gCosts;
    
    // Track parent nodes for path reconstruction
    std::unordered_map<int, int> cameFrom;
    
    auto startNode = graph->GetNode(startId);
    auto goalNode = graph->GetNode(goalId);
    
    if (!startNode || !goalNode) {
        std::cerr << "Invalid start or goal node" << std::endl;
        return {};
    }
    
    // Initialize start node
    float hCost = Heuristic(startNode->position, goalNode->position);
    openSet.push(AStarNode(startId, 0.0f, hCost, -1));
    gCosts[startId] = 0.0f;
    
    while (!openSet.empty()) {
        // Get node with lowest fCost
        AStarNode current = openSet.top();
        openSet.pop();
        
        // Skip if already processed
        if (closedSet.count(current.nodeId)) {
            continue;
        }
        
        // Mark as processed
        closedSet.insert(current.nodeId);
        
        // Check if we reached the goal
        if (current.nodeId == goalId) {
            // Reconstruct path
            std::vector<int> path;
            int nodeId = goalId;
            while (nodeId != -1) {
                path.push_back(nodeId);
                auto it = cameFrom.find(nodeId);
                nodeId = (it != cameFrom.end()) ? it->second : -1;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Explore neighbors
        auto currentNode = graph->GetNode(current.nodeId);
        for (const auto& edge : currentNode->edges) {
            int neighborId = edge->to->id;
            
            // Skip if already processed
            if (closedSet.count(neighborId)) {
                continue;
            }
            
            // Calculate tentative gCost
            float tentativeGCost = current.gCost + edge->weight;
            
            // Check if this path to neighbor is better
            auto it = gCosts.find(neighborId);
            if (it == gCosts.end() || tentativeGCost < it->second) {
                // Update best path to neighbor
                gCosts[neighborId] = tentativeGCost;
                cameFrom[neighborId] = current.nodeId;
                
                float hCost = Heuristic(edge->to->position, goalNode->position);
                openSet.push(AStarNode(neighborId, tentativeGCost, hCost, current.nodeId));
            }
        }
    }
    
    // No path found
    std::cerr << "No path found from " << startId << " to " << goalId << std::endl;
    return {};
}
