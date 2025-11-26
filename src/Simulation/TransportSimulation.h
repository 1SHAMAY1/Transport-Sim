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
    
private:
    void CreateRoadNetwork();
    void SpawnInitialVehicles();
    
    std::shared_ptr<Graph> m_Graph;
    std::vector<std::shared_ptr<Vehicle>> m_Vehicles;
    int m_NextVehicleId = 0;
};
