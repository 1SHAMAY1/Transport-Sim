#pragma once
#include "Graph.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <algorithm>

// A* Pathfinding implementation
class Pathfinding {
public:
    // Find shortest path from start to goal using A* algorithm
    // Returns list of node IDs forming the path (empty if no path found)
    static std::vector<int> AStar(
        std::shared_ptr<Graph> graph,
        int startId,
        int goalId
    );
    
private:
    // Heuristic function (Euclidean distance)
    static float Heuristic(const glm::vec3& a, const glm::vec3& b);
    
    // Node data for A* algorithm
    struct AStarNode {
        int nodeId;
        float gCost;  // Cost from start to current node
        float hCost;  // Heuristic cost from current to goal
        float fCost;  // Total cost (g + h)
        int parentId; // Parent node in path
        
        AStarNode(int id, float g, float h, int parent)
            : nodeId(id), gCost(g), hCost(h), fCost(g + h), parentId(parent) {}
        
        // For priority queue (min-heap based on fCost)
        bool operator>(const AStarNode& other) const {
            return fCost > other.fCost;
        }
    };
};
