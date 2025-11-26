#include "Vehicle.h"
#include <glm/gtc/constants.hpp>

Vehicle::Vehicle(int id, const glm::vec3& position)
    : m_Id(id), m_Position(position), m_Velocity(0.0f) {
}

void Vehicle::Update(float deltaTime) {
    if (m_Path.empty() || m_CurrentWaypointIndex >= m_Path.size()) {
        m_Velocity = glm::vec3(0.0f);
        return;
    }
    
    const glm::vec3& targetWaypoint = m_Path[m_CurrentWaypointIndex];
    glm::vec3 direction = targetWaypoint - m_Position;
    float distance = glm::length(direction);
    
    // Check if we've reached the current waypoint
    if (distance < 0.1f) {
        m_CurrentWaypointIndex++;
        if (m_CurrentWaypointIndex >= m_Path.size()) {
            // Reached end of path
            m_Velocity = glm::vec3(0.0f);
            m_Path.clear();
            return;
        }
        return;  // Move to next waypoint in next frame
    }
    
    // Move towards waypoint
    direction = glm::normalize(direction);
    m_Velocity = direction * m_Speed;
    m_Position += m_Velocity * deltaTime;
}

void Vehicle::SetPath(const std::vector<int>& path, std::shared_ptr<Graph> graph) {
    m_Path.clear();
    m_CurrentWaypointIndex = 0;
    
    // Convert node IDs to positions
    for (int nodeId : path) {
        auto node = graph->GetNode(nodeId);
        if (node) {
            m_Path.push_back(node->position);
        }
    }
}
