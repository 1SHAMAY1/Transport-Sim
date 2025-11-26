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
    // Create massive 50x50 grid
    const int gridSize = 50;
    const float spacing = 10.0f;
    
    // Create nodes (intersections)
    std::vector<std::vector<int>> nodeGrid(gridSize, std::vector<int>(gridSize));
    
    for (int x = 0; x < gridSize; x++) {
        for (int z = 0; z < gridSize; z++) {
            glm::vec3 position(x * spacing, 0.0f, z * spacing);
            int nodeId = m_Graph->AddNode(position);
            nodeGrid[x][z] = nodeId;
            
            // Randomize traffic light durations
            auto node = m_Graph->GetNode(nodeId);
            if (node) {
                node->greenDuration = 5.0f + (rand() % 5); // 5-10 seconds
                node->redDuration = 5.0f + (rand() % 5);
            }
        }
    }
    
    // Create roads with mix of one-way and two-way
    for (int x = 0; x < gridSize; x++) {
        for (int z = 0; z < gridSize; z++) {
            // Horizontal roads
            if (x < gridSize - 1) {
                if (z % 2 == 0) {
                    m_Graph->AddBidirectionalEdge(nodeGrid[x][z], nodeGrid[x + 1][z], spacing);
                } else {
                    m_Graph->AddEdge(nodeGrid[x][z], nodeGrid[x + 1][z], spacing);
                }
            }
            
            // Vertical roads
            if (z < gridSize - 1) {
                if (x % 2 == 0) {
                    m_Graph->AddBidirectionalEdge(nodeGrid[x][z], nodeGrid[x][z + 1], spacing);
                } else {
                    m_Graph->AddEdge(nodeGrid[x][z], nodeGrid[x][z + 1], spacing);
                }
            }
        }
    }
    
    // Add selective diagonal shortcuts (less frequent for large grid)
    float diagonalDist = spacing * sqrt(2.0f);
    
    for (int x = 0; x < gridSize - 1; x += 4) { // Every 4th block
        for (int z = 0; z < gridSize - 1; z += 4) {
            m_Graph->AddBidirectionalEdge(nodeGrid[x][z], nodeGrid[x + 1][z + 1], diagonalDist);
            if (x + 1 < gridSize && z + 1 < gridSize) {
                m_Graph->AddBidirectionalEdge(nodeGrid[x + 1][z], nodeGrid[x][z + 1], diagonalDist);
            }
        }
    }
    
    std::cout << "Created road network with " << m_Graph->GetNodeCount() << " nodes" << std::endl;
    std::cout << "Grid: " << gridSize << "x" << gridSize << std::endl;
}

void TransportSimulation::SpawnInitialVehicles() {
    // Initial spawn
    const int initialVehicles = 100;
    for (int i = 0; i < initialVehicles; i++) {
        SpawnVehicle();
    }
}

void TransportSimulation::SpawnVehicle() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, (int)m_Graph->GetNodeCount() - 1);
    
    int startNodeId = dis(gen);
    auto startNode = m_Graph->GetNode(startNodeId);
    if (!startNode) return;
    
    // Find a valid goal node within distance range (5-70 blocks)
    // Since grid spacing is 10.0f, 5 blocks = 50.0f, 70 blocks = 700.0f
    int goalNodeId = -1;
    int attempts = 0;
    
    while (attempts < 20) {
        int candidateId = dis(gen);
        if (candidateId == startNodeId) continue;
        
        auto candidateNode = m_Graph->GetNode(candidateId);
        if (!candidateNode) continue;
        
        // Manhattan distance approximation for grid blocks
        float dist = glm::length(candidateNode->position - startNode->position);
        float blocks = dist / 10.0f; // spacing is 10.0f
        
        if (blocks >= 5.0f && blocks <= 70.0f) {
            goalNodeId = candidateId;
            break;
        }
        attempts++;
    }
    
    if (goalNodeId == -1) return; // Could not find valid goal
    
    auto vehicle = std::make_shared<Vehicle>(m_NextVehicleId++, startNode->position);
    
    auto path = Pathfinding::AStar(m_Graph, startNodeId, goalNodeId);
    if (!path.empty()) {
        vehicle->SetPath(path, m_Graph);
        m_Vehicles.push_back(vehicle);
    }
}

void TransportSimulation::Update(float deltaTime) {
    // 1. Update Traffic Lights
    for (const auto& [id, node] : m_Graph->GetNodes()) {
        node->lightTimer += deltaTime;
        
        if (node->lightState == TrafficLightState::GREEN) {
            if (node->lightTimer >= node->greenDuration) {
                node->lightState = TrafficLightState::YELLOW; // Transition to Yellow first
                node->lightTimer = 0.0f;
            }
        } else if (node->lightState == TrafficLightState::YELLOW) {
             if (node->lightTimer >= 2.0f) { // 2 seconds yellow
                node->lightState = TrafficLightState::RED;
                node->lightTimer = 0.0f;
            }
        } else if (node->lightState == TrafficLightState::RED) {
            if (node->lightTimer >= node->redDuration) {
                node->lightState = TrafficLightState::GREEN;
                node->lightTimer = 0.0f;
            }
        }
    }

    // 2. Update Vehicles
    for (auto& vehicle : m_Vehicles) {
        vehicle->Update(deltaTime, m_Graph);
    }
    
    // 3. Collision Avoidance
    const float safeDistance = 2.0f;
    for (size_t i = 0; i < m_Vehicles.size(); i++) {
        auto& vehicleA = m_Vehicles[i];
        if (vehicleA->IsStopped()) continue; // Already stopped at light
        
        bool tooClose = false;
        for (size_t j = 0; j < m_Vehicles.size(); j++) {
            if (i == j) continue;
            auto& vehicleB = m_Vehicles[j];
            
            float dist = glm::length(vehicleA->GetPosition() - vehicleB->GetPosition());
            if (dist < safeDistance) {
                tooClose = true;
                break;
            }
        }
        
        if (tooClose) {
            vehicleA->SetSpeed(0.0f); // Stop for traffic jam
        } else {
            vehicleA->SetSpeed(5.0f); // Resume speed
        }
    }
    
    // 4. Vehicle Lifecycle (Destroy & Respawn)
    auto it = m_Vehicles.begin();
    while (it != m_Vehicles.end()) {
        if (!(*it)->IsMoving()) {
            it = m_Vehicles.erase(it);
        } else {
            ++it;
        }
    }
    
    // Respawn to maintain constant count
    const int targetVehicles = 100;
    
    while (m_Vehicles.size() < targetVehicles) {
        SpawnVehicle();
    }
}

void TransportSimulation::AddVehicle(int startNodeId) {
    auto startNode = m_Graph->GetNode(startNodeId);
    if (!startNode) return;
    
    auto vehicle = std::make_shared<Vehicle>(m_NextVehicleId++, startNode->position);
    m_Vehicles.push_back(vehicle);
}
