#include "Vehicle.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

Vehicle::Vehicle(int id, const glm::vec3& position)
    : m_Id(id), m_Position(position), m_Velocity(0.0f) {
}

void Vehicle::Update(float deltaTime, std::shared_ptr<Graph> graph) {
    if (m_Path.empty() || m_CurrentWaypointIndex >= m_Path.size()) {
        m_Velocity = glm::vec3(0.0f);
        return;
    }
    
    // Check traffic light at the target intersection
    m_IsStopped = false;
    if (m_CurrentWaypointIndex < m_NodePath.size()) {
        int targetNodeId = m_NodePath[m_CurrentWaypointIndex];
        auto node = graph->GetNode(targetNodeId);
        
        // We need to know where we are coming FROM to check the correct light
        // If we are at the start, we might not have a previous node in path, 
        // but we can infer it or check if we are close to the target.
        // Better: Use the previous node in the path if available.
        int fromNodeId = -1;
        if (m_CurrentWaypointIndex > 0) {
            fromNodeId = m_NodePath[m_CurrentWaypointIndex - 1];
        } else {
            // Special case: Just spawned or at start. 
            // We can try to find the closest neighbor that aligns with our direction?
            // For now, let's assume if we are just starting, we are "entering" the network 
            // and might not need to check a light immediately unless we are right at an intersection.
            // But usually we spawn at a node and move to next.
        }

        if (node && fromNodeId != -1) {
            // Check if there is a light for our incoming path
            auto it = node->incomingLights.find(fromNodeId);
            if (it != node->incomingLights.end() && it->second == TrafficLightState::RED) {
                // Check distance to intersection
                float distToNode = glm::length(node->position - m_Position);
                if (distToNode < 6.0f) {  // Stop before the intersection
                    m_IsStopped = true;
                    m_Velocity = glm::vec3(0.0f);
                    // Debug Log

                    return;
                }
            }
        }
    }
    
    const glm::vec3& targetWaypoint = m_Path[m_CurrentWaypointIndex];
    glm::vec3 direction = targetWaypoint - m_Position;
    float distance = glm::length(direction);
    
    // Check if we've reached the current waypoint
    if (distance < 0.5f) {
        m_CurrentWaypointIndex++;
        if (m_CurrentWaypointIndex >= m_Path.size()) {
            // Reached end of path
            m_DestinationReached = true;
            m_Velocity = glm::vec3(0.0f);
            m_Path.clear(); // Signal for destruction
            m_NodePath.clear();
            return;
        }
        return;  // Move to next waypoint in next frame
    }
    
    // Move towards waypoint
    direction = glm::normalize(direction);
    m_Direction = direction; // Update direction
    m_Velocity = direction * m_Speed;
    m_Position += m_Velocity * deltaTime;
}

void Vehicle::SetPath(const std::vector<int>& path, std::shared_ptr<Graph> graph) {
    m_Path.clear();
    m_NodePath = path;  // Store node IDs
    m_CurrentWaypointIndex = 0;
    m_IsStopped = false;
    m_DestinationReached = false;
    m_WaitTimer = 0.0f;
    
    // Convert node IDs to positions
    // Lane Offset Logic
    float laneOffset = 0.1f; // Offset to the right (Road width is 0.4, lane width 0.2, center at 0.1)
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    for (size_t i = 0; i < path.size(); ++i) {
        auto node = graph->GetNode(path[i]);
        if (!node) continue;

        glm::vec3 position = node->position;
        glm::vec3 dir(0.0f);

        // Determine direction for offset
        if (i < path.size() - 1) {
            // Use direction to next node
            auto nextNode = graph->GetNode(path[i+1]);
            if (nextNode) {
                dir = glm::normalize(nextNode->position - node->position);
            }
        } else if (i > 0) {
            // Last node: Use direction from previous node
            auto prevNode = graph->GetNode(path[i-1]);
            if (prevNode) {
                dir = glm::normalize(node->position - prevNode->position);
            }
        }

        // Apply offset if we have a valid direction
        if (glm::length(dir) > 0.01f) {
            glm::vec3 right = glm::normalize(glm::cross(dir, up));
            position += right * laneOffset;
        }

        m_Path.push_back(position);
    }
    
    // Set initial direction and position (snap to first waypoint)
    if (!m_Path.empty()) {
        m_Position = m_Path[0]; // Snap to lane center
        if (m_Path.size() > 1) {
            m_Direction = glm::normalize(m_Path[1] - m_Position);
        }
    }
}
