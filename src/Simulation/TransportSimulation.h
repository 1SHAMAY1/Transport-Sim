#pragma once
#include "Graph.h"
#include "Vehicle.h"
#include "Pathfinding.h"
#include <memory>
#include <vector>

// Manages the entire transport simulation
class TransportSimulation {
public:
    TransportSimulation();
    ~TransportSimulation() = default;
    
    void Initialize();
    void Update(float deltaTime);
    
    // Getters
    std::shared_ptr<Graph> GetGraph() const { return m_Graph; }
    const std::vector<std::shared_ptr<Vehicle>>& GetVehicles() const { return m_Vehicles; }
    
    // Add a vehicle at a specific node
    void AddVehicle(int startNodeId);
    void SpawnVehicle();
    
    // Traffic Light Control
    void SetTrafficLightsEnabled(bool enabled);
    bool AreTrafficLightsEnabled() const { return m_TrafficLightsEnabled; }

private:
    void CreateRoadNetwork();
    void SpawnInitialVehicles();
    
    std::shared_ptr<Graph> m_Graph;
    std::shared_ptr<Pathfinding> m_Pathfinding;
    
    std::vector<std::shared_ptr<Vehicle>> m_Vehicles;
    float m_SpawnTimer = 0.0f;
    int m_NextVehicleId = 0;
    
    // Spawn Queue
    struct SpawnRequest {
        float timer;
    };
    std::vector<SpawnRequest> m_SpawnQueue;
    
    bool m_TrafficLightsEnabled = true;
};
