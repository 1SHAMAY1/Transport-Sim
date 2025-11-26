#include "TransportSimulation.h"
#include <iostream>
#include <random>

TransportSimulation::TransportSimulation() {
    m_Graph = std::make_shared<Graph>();
}

void TransportSimulation::Initialize() {
    CreateRoadNetwork();
    SpawnInitialVehicles();
}

void TransportSimulation::CreateRoadNetwork() {
    // Create larger single-level grid with one-way and two-way roads
    const int gridSize = 12;  // Increased to 12x12
    const float spacing = 10.0f;
    
    // Create nodes (intersections)
    std::vector<std::vector<int>> nodeGrid(gridSize, std::vector<int>(gridSize));
    
    for (int x = 0; x < gridSize; x++) {
        for (int z = 0; z < gridSize; z++) {
            glm::vec3 position(x * spacing, 0.0f, z * spacing);
            int nodeId = m_Graph->AddNode(position);
            nodeGrid[x][z] = nodeId;
        }
    }
    
    // Create roads with mix of one-way and two-way
    for (int x = 0; x < gridSize; x++) {
        for (int z = 0; z < gridSize; z++) {
            // Horizontal roads - alternating one-way and two-way
            if (x < gridSize - 1) {
                if (z % 2 == 0) {
                    // Two-way road
                    m_Graph->AddBidirectionalEdge(nodeGrid[x][z], nodeGrid[x + 1][z], spacing);
                } else {
                    // One-way road (left to right)
                    m_Graph->AddEdge(nodeGrid[x][z], nodeGrid[x + 1][z], spacing);
                }
            }
            
            // Vertical roads - alternating one-way and two-way (opposite pattern)
            if (z < gridSize - 1) {
                if (x % 2 == 0) {
                    // Two-way road
                    m_Graph->AddBidirectionalEdge(nodeGrid[x][z], nodeGrid[x][z + 1], spacing);
                } else {
                    // One-way road (bottom to top)
                    m_Graph->AddEdge(nodeGrid[x][z], nodeGrid[x][z + 1], spacing);
                }
            }
        }
    }
    
    // Add selective diagonal shortcuts (all two-way)
    float diagonalDist = spacing * sqrt(2.0f);
    
    for (int x = 0; x < gridSize - 1; x += 2) {
        for (int z = 0; z < gridSize - 1; z += 2) {
            // Diagonal shortcuts are bidirectional for better routing
            m_Graph->AddBidirectionalEdge(
                nodeGrid[x][z],
                nodeGrid[x + 1][z + 1],
                diagonalDist
            );
            
            if (x + 1 < gridSize && z + 1 < gridSize) {
                m_Graph->AddBidirectionalEdge(
                    nodeGrid[x + 1][z],
                    nodeGrid[x][z + 1],
                    diagonalDist
                );
            }
        }
    }
    
    std::cout << "Created road network with " << m_Graph->GetNodeCount() << " nodes" << std::endl;
    std::cout << "Grid: " << gridSize << "x" << gridSize << " (single level)" << std::endl;
    std::cout << "Features: One-way + Two-way roads + Diagonal shortcuts" << std::endl;
}

void TransportSimulation::SpawnInitialVehicles() {
    // Spawn more vehicles for larger network
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, (int)m_Graph->GetNodeCount() - 1);
    
    const int numVehicles = 25;  // Increased for larger grid
    
    for (int i = 0; i < numVehicles; i++) {
        int startNodeId = dis(gen);
        int goalNodeId = dis(gen);
        
        // Ensure start and goal are different
        while (goalNodeId == startNodeId) {
            goalNodeId = dis(gen);
        }
        
        auto startNode = m_Graph->GetNode(startNodeId);
        if (!startNode) continue;
        
        auto vehicle = std::make_shared<Vehicle>(m_NextVehicleId++, startNode->position);
        
        // Find path using A*
        auto path = Pathfinding::AStar(m_Graph, startNodeId, goalNodeId);
        if (!path.empty()) {
            vehicle->SetPath(path, m_Graph);
            m_Vehicles.push_back(vehicle);
            std::cout << "Spawned vehicle " << vehicle->GetId() 
                      << " with path from " << startNodeId << " to " << goalNodeId << std::endl;
        }
    }
}

void TransportSimulation::Update(float deltaTime) {
    // Update all vehicles
    for (auto& vehicle : m_Vehicles) {
        vehicle->Update(deltaTime);
    }
    
    // Collision avoidance - check proximity between vehicles
    const float safeDistance = 2.0f;  // Minimum safe distance
    
    for (size_t i = 0; i < m_Vehicles.size(); i++) {
        auto& vehicleA = m_Vehicles[i];
        bool tooClose = false;
        
        // Check against all other vehicles
        for (size_t j = 0; j < m_Vehicles.size(); j++) {
            if (i == j) continue;
            
            auto& vehicleB = m_Vehicles[j];
            float dist = glm::length(vehicleA->GetPosition() - vehicleB->GetPosition());
            
            if (dist < safeDistance && vehicleA->IsMoving()) {
                tooClose = true;
                break;
            }
        }
        
        // Adjust speed based on proximity
        if (tooClose) {
            vehicleA->SetSpeed(1.0f);  // Slow down
        } else {
            vehicleA->SetSpeed(5.0f);  // Normal speed
        }
    }
    
    // Check if vehicles reached their destination and assign new routes
    for (auto& vehicle : m_Vehicles) {
        if (!vehicle->IsMoving()) {
            // Assign new random destination
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, (int)m_Graph->GetNodeCount() - 1);
            
            // Find closest node to current position
            int startNodeId = 0;
            float minDist = std::numeric_limits<float>::max();
            for (const auto& [id, node] : m_Graph->GetNodes()) {
                float dist = glm::length(node->position - vehicle->GetPosition());
                if (dist < minDist) {
                    minDist = dist;
                    startNodeId = id;
                }
            }
            
            int goalNodeId = dis(gen);
            while (goalNodeId == startNodeId) {
                goalNodeId = dis(gen);
            }
            
            auto path = Pathfinding::AStar(m_Graph, startNodeId, goalNodeId);
            if (!path.empty()) {
                vehicle->SetPath(path, m_Graph);
            }
        }
    }
}

void TransportSimulation::AddVehicle(int startNodeId) {
    auto startNode = m_Graph->GetNode(startNodeId);
    if (!startNode) return;
    
    auto vehicle = std::make_shared<Vehicle>(m_NextVehicleId++, startNode->position);
    m_Vehicles.push_back(vehicle);
}
