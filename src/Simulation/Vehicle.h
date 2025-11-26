#pragma once
#include "Graph.h"
#include <glm/glm.hpp>
#include <vector>

// Vehicle agent that moves through the network
class Vehicle {
public:
    Vehicle(int id, const glm::vec3& position);
    ~Vehicle() = default;
    
    // Update vehicle position (move along path)
    void Update(float deltaTime);
    
    // Set a new path for the vehicle to follow
    void SetPath(const std::vector<int>& path, std::shared_ptr<Graph> graph);
    
    // Getters
    int GetId() const { return m_Id; }
    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3& GetVelocity() const { return m_Velocity; }
    float GetSpeed() const { return m_Speed; }
    bool IsMoving() const { return !m_Path.empty(); }
    
    // Setters
    void SetSpeed(float speed) { m_Speed = speed; }
    
private:
    int m_Id;
    glm::vec3 m_Position;
    glm::vec3 m_Velocity;
    float m_Speed = 5.0f;  // Units per second
    
    std::vector<glm::vec3> m_Path;  // Waypoints to follow
    size_t m_CurrentWaypointIndex = 0;
};
