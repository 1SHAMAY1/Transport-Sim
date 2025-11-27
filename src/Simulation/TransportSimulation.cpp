#include "TransportSimulation.h"
#include <iostream>
#include <random>
#include <algorithm>

TransportSimulation::TransportSimulation() {
    m_Graph = std::make_shared<Graph>();
}

void TransportSimulation::Initialize() {
    CreateRoadNetwork();
    SpawnInitialVehicles();
}

void TransportSimulation::CreateRoadNetwork() {
    // Create 20x20 grid
    const int gridSize = 20;
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
    
    // Initialize Traffic Lights (Per-Path)
    for (const auto& [id, node] : m_Graph->GetNodes()) {
        // Find all incoming edges to this node
        std::vector<int> incomingNeighbors;
        for (const auto& [otherId, otherNode] : m_Graph->GetNodes()) {
            for (const auto& edge : otherNode->edges) {
                if (edge->to->id == id) {
                    incomingNeighbors.push_back(otherId);
                }
            }
        }
        
        // If it's an intersection (more than 1 incoming road), add lights
        if (incomingNeighbors.size() > 1 && rand() % 4 == 0) { // 25% chance
            for (int neighborId : incomingNeighbors) {
                node->incomingLights[neighborId] = TrafficLightState::RED;
            }
            
            // Set one random neighbor to GREEN initially
            if (!incomingNeighbors.empty()) {
                int greenIdx = rand() % incomingNeighbors.size();
                node->currentGreenNodeId = incomingNeighbors[greenIdx];
                node->incomingLights[node->currentGreenNodeId] = TrafficLightState::GREEN;
            }
        } else {
            // No lights (OFF)
            for (int neighborId : incomingNeighbors) {
                node->incomingLights[neighborId] = TrafficLightState::OFF;
            }
        }
    }
    
    std::cout << "Created road network with " << m_Graph->GetNodeCount() << " nodes" << std::endl;
    std::cout << "Grid: " << gridSize << "x" << gridSize << std::endl;
}

void TransportSimulation::SpawnInitialVehicles() {
    // Initial spawn
    const int initialVehicles = 150;
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
    
    // Check if node is already occupied
    for (const auto& v : m_Vehicles) {
        if (glm::length(v->GetPosition() - startNode->position) < 5.0f) {
            return; // Node occupied, skip spawn
        }
    }
    
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

// Helper to count vehicles on a specific edge (approaching 'toNode' from 'fromNode')
int GetVehicleCountOnEdge(const std::vector<std::shared_ptr<Vehicle>>& vehicles, int fromId, int toId, std::shared_ptr<Graph> graph) {
    int count = 0;
    auto fromNode = graph->GetNode(fromId);
    auto toNode = graph->GetNode(toId);
    if (!fromNode || !toNode) return 0;
    
    for (const auto& v : vehicles) {
        if (v->IsDestinationReached()) continue;
        
        // Check if vehicle is on this edge segment
        // Simple check: Vehicle is close to fromNode or between fromNode and toNode
        // Better check: Look at vehicle's current path target
        const auto& path = v->GetNodePath();
        size_t idx = v->GetCurrentWaypointIndex();
        
        if (idx < path.size()) {
            int targetId = path[idx];
            // If vehicle is targeting 'toNode'
            if (targetId == toId) {
                // And it's coming from 'fromNode' direction (check distance)
                float distToTarget = glm::length(v->GetPosition() - toNode->position);
                float distFromSource = glm::length(v->GetPosition() - fromNode->position);
                float edgeLen = glm::length(toNode->position - fromNode->position);
                
                // If within the edge bounds
                if (distToTarget <= edgeLen && distFromSource <= edgeLen) {
                    count++;
                }
            }
        }
    }
    return count;
}

void TransportSimulation::Update(float deltaTime) {
    // 1. Update Traffic Lights (Sensor Based)
    if (m_TrafficLightsEnabled) {
        for (const auto& [id, node] : m_Graph->GetNodes()) {
            // Skip nodes without active lights
            bool hasActiveLights = false;
            for (const auto& [neighbor, state] : node->incomingLights) {
                if (state != TrafficLightState::OFF) {
                    hasActiveLights = true;
                    break;
                }
            }
            if (!hasActiveLights) continue;
            
            node->lightTimer += deltaTime;
            
            // State Machine for the Intersection
            // We only switch phases if:
            // a) Current green lane is empty AND another lane has cars
            // b) Max green duration exceeded AND another lane has cars
            // c) Yellow phase complete
            
            // Find current green neighbor
            int currentGreen = node->currentGreenNodeId;
            
            // Check if we are in Yellow phase
            bool isYellow = false;
            if (currentGreen != -1 && node->incomingLights[currentGreen] == TrafficLightState::YELLOW) {
                isYellow = true;
            }
            
            if (isYellow) {
                if (node->lightTimer >= 2.0f) {
                    // Switch to Red, then pick next Green
                    node->incomingLights[currentGreen] = TrafficLightState::RED;
                    
                    // Pick next green based on sensor (most cars)
                    int bestNeighbor = -1;
                    int maxCars = -1;
                    
                    for (const auto& [neighbor, state] : node->incomingLights) {
                        if (neighbor == currentGreen) continue; // Don't pick same again immediately
                        
                        int cars = GetVehicleCountOnEdge(m_Vehicles, neighbor, id, m_Graph);
                        if (cars > maxCars) {
                            maxCars = cars;
                            bestNeighbor = neighbor;
                        }
                    }
                    
                    // If no cars found anywhere, just pick random next or keep red?
                    // Let's pick random if no cars to keep cycle moving (or just wait)
                    if (bestNeighbor == -1) {
                        // Pick first available
                        for (const auto& [neighbor, state] : node->incomingLights) {
                            if (neighbor != currentGreen) {
                                bestNeighbor = neighbor;
                                break;
                            }
                        }
                    }
                    
                    if (bestNeighbor != -1) {
                        node->currentGreenNodeId = bestNeighbor;
                        node->incomingLights[bestNeighbor] = TrafficLightState::GREEN;
                        node->lightTimer = 0.0f;
                    }
                }
            } else {
                // Currently Green
                if (currentGreen != -1) {
                    int carsOnGreen = GetVehicleCountOnEdge(m_Vehicles, currentGreen, id, m_Graph);
                    
                    // Check other lanes
                    int maxCarsOther = 0;
                    for (const auto& [neighbor, state] : node->incomingLights) {
                        if (neighbor != currentGreen) {
                            int cars = GetVehicleCountOnEdge(m_Vehicles, neighbor, id, m_Graph);
                            if (cars > maxCarsOther) maxCarsOther = cars;
                        }
                    }
                    
                    bool shouldSwitch = false;
                    
                    // Rule 1: Empty Green Lane & Waiting Cars elsewhere
                    if (carsOnGreen == 0 && maxCarsOther > 0 && node->lightTimer > node->minGreenDuration) {
                        shouldSwitch = true;
                    }
                    
                    // Rule 2: Max Duration Exceeded & Waiting Cars elsewhere
                    if (node->lightTimer > node->maxGreenDuration && maxCarsOther > 0) {
                        shouldSwitch = true;
                    }
                    
                    if (shouldSwitch) {
                        node->incomingLights[currentGreen] = TrafficLightState::YELLOW;
                        node->lightTimer = 0.0f;
                    }
                }
            }
        }
    }

    // 2. Update Vehicles
    for (auto& vehicle : m_Vehicles) {
        vehicle->Update(deltaTime, m_Graph);
    }
    
    // 3. Collision Avoidance & Junction Logic
    const float safeDistance = 4.0f; 
    const float criticalDistance = 1.5f;

    for (size_t i = 0; i < m_Vehicles.size(); i++) {
        auto& vehicleA = m_Vehicles[i];
        if (vehicleA->IsStopped()) continue; // Already stopped at red light
        
        bool shouldStop = false;
        float targetSpeed = 5.0f;
        bool isBlockedByVehicle = false;
        
        // 0. Don't Block the Box (Gridlock Prevention)
        // Check if the NEXT edge (after the intersection) is full
        const auto& path = vehicleA->GetNodePath();
        size_t idx = vehicleA->GetCurrentWaypointIndex();
        
        // Only check if we are approaching an intersection (target node)
        if (idx < path.size()) {
            int targetNodeId = path[idx];
            auto targetNode = m_Graph->GetNode(targetNodeId);
            
            if (targetNode) {
                float distToIntersection = glm::length(targetNode->position - vehicleA->GetPosition());
                
                // If we are close to entering the intersection (e.g. < 15.0f)
                if (distToIntersection < 15.0f) {
                    // Check the NEXT edge
                    if (idx + 1 < path.size()) {
                        int nextNodeId = path[idx + 1];
                        auto nextNode = m_Graph->GetNode(nextNodeId);
                        
                        if (nextNode) {
                            // Calculate capacity of the target edge
                            float edgeLen = glm::length(nextNode->position - targetNode->position);
                            int capacity = (int)(edgeLen / 8.0f); // Assume ~8 units per car (incl gap)
                            
                            // Count cars on that edge
                            int carsOnNextEdge = GetVehicleCountOnEdge(m_Vehicles, targetNodeId, nextNodeId, m_Graph);
                            
                            if (carsOnNextEdge >= capacity) {
                                shouldStop = true;
                                // std::cout << "Vehicle " << vehicleA->GetId() << " waiting for gridlock at " << targetNodeId << std::endl;
                            }
                        }
                    }
                }
            }
        }
        
        // A. Check against other vehicles
        for (size_t j = 0; j < m_Vehicles.size(); j++) {
            if (i == j) continue;
            auto& vehicleB = m_Vehicles[j];
            
            if (vehicleB->IsDestinationReached()) continue;
            
            glm::vec3 toOther = vehicleB->GetPosition() - vehicleA->GetPosition();
            float dist = glm::length(toOther);
            
            // 0. Ignore oncoming traffic (Head-on on two-way roads)
            // If vehicles are moving in opposite directions (dot product < -0.5), they are in different lanes
            if (glm::dot(vehicleA->GetDirection(), vehicleB->GetDirection()) < -0.5f) {
                continue;
            }
            
            // 1. Critical Proximity (Anti-Clipping) - Absolute stop
            if (dist < criticalDistance) {
                shouldStop = true;
                isBlockedByVehicle = true;
                break;
            }
            
            // 2. Standard Following Distance
            if (dist < safeDistance) {
                // Check if B is strictly in front (narrower cone)
                // 0.8f is approx 37 degrees
                if (glm::dot(glm::normalize(toOther), vehicleA->GetDirection()) > 0.8f) {
                    
                    // Lateral Distance Check (Lane Logic)
                    glm::vec3 right = glm::cross(vehicleA->GetDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
                    float lateralDist = std::abs(glm::dot(toOther, right));
                    
                    // If lateral distance is significant (different lane/offset), don't stop completely
                    if (lateralDist > 1.5f) { // 1.5f allows for some passing
                         if (lateralDist < 3.0f) {
                             // Tight squeeze, slow down
                             targetSpeed = std::min(targetSpeed, 2.5f);
                         }
                         // If > 3.0f, ignore (safe pass)
                    } else {
                        // Same lane, must stop
                        shouldStop = true;
                        isBlockedByVehicle = true;
                        break;
                    }
                }
            }
        }
        
        if (shouldStop) {
            // "Wait 5s then Pass" Logic
            // Only apply if blocked by vehicle (not red light) and we are stopped
            if (isBlockedByVehicle) {
                vehicleA->IncrementBlockedTimer(deltaTime);
                
                if (vehicleA->GetBlockedTimer() > 5.0f) {
                    // Creep forward slowly to attempt pass
                    vehicleA->SetSpeed(1.0f); 
                    // Debug Log (occasional)
                    if (vehicleA->GetId() % 20 == 0) {
                        // std::cout << "Vehicle " << vehicleA->GetId() << " forcing pass after wait." << std::endl;
                    }
                } else {
                    vehicleA->SetSpeed(0.0f);
                }
            } else {
                // Blocked by red light or other reason, reset timer
                vehicleA->ResetBlockedTimer();
                vehicleA->SetSpeed(0.0f);
            }
        } else {
            vehicleA->ResetBlockedTimer();
            vehicleA->SetSpeed(targetSpeed);
        }
    }
    
    // Debug: Print total stopped vehicles periodically
    static float logTimer = 0.0f;
    logTimer += deltaTime;
    if (logTimer > 1.0f) {
        int stoppedCount = 0;
        for (const auto& v : m_Vehicles) {
            if (v->GetSpeed() < 0.1f) stoppedCount++;
        }
        std::cout << "Active Vehicles: " << m_Vehicles.size() << " | Stopped: " << stoppedCount << std::endl;
        logTimer = 0.0f;
    }
    
    // 4. Vehicle Lifecycle (Destroy & Respawn)
    auto it = m_Vehicles.begin();
    while (it != m_Vehicles.end()) {
        if ((*it)->IsDestinationReached()) {
            // Add to spawn queue with delay
            m_SpawnQueue.push_back({ 5.0f }); // 5 second delay
            it = m_Vehicles.erase(it);
        } else {
            ++it;
        }
    }
    
    // Process Spawn Queue
    for (auto& req : m_SpawnQueue) {
        req.timer -= deltaTime;
    }
    
    // Remove ready spawns and spawn vehicles
    auto readyIt = std::remove_if(m_SpawnQueue.begin(), m_SpawnQueue.end(), 
        [this](const SpawnRequest& req) {
            if (req.timer <= 0.0f) {
                SpawnVehicle();
                return true;
            }
            return false;
        });
    m_SpawnQueue.erase(readyIt, m_SpawnQueue.end());
    
    // Maintain vehicle count (cap at 200)
    size_t totalVehicles = m_Vehicles.size() + m_SpawnQueue.size();
    if (totalVehicles < 200) {
        SpawnVehicle();
    }
}

void TransportSimulation::AddVehicle(int startNodeId) {
    auto startNode = m_Graph->GetNode(startNodeId);
    if (!startNode) return;
    
    auto vehicle = std::make_shared<Vehicle>(m_NextVehicleId++, startNode->position);
    m_Vehicles.push_back(vehicle);
}

void TransportSimulation::SetTrafficLightsEnabled(bool enabled) {
    m_TrafficLightsEnabled = enabled;
    
    // If disabled, turn off all lights
    if (!enabled) {
        for (auto& [id, node] : m_Graph->GetNodes()) {
            for (auto& [neighbor, state] : node->incomingLights) {
                state = TrafficLightState::OFF;
            }
        }
    } else {
        // Re-initialize lights
        for (auto& [id, node] : m_Graph->GetNodes()) {
            // Check if it should have lights (same logic as creation)
             std::vector<int> incomingNeighbors;
            for (const auto& [otherId, otherNode] : m_Graph->GetNodes()) {
                for (const auto& edge : otherNode->edges) {
                    if (edge->to->id == id) {
                        incomingNeighbors.push_back(otherId);
                    }
                }
            }
            
            if (incomingNeighbors.size() > 1 && rand() % 4 == 0) {
                for (int neighborId : incomingNeighbors) {
                    node->incomingLights[neighborId] = TrafficLightState::RED;
                }
                if (!incomingNeighbors.empty()) {
                    int greenIdx = rand() % incomingNeighbors.size();
                    node->currentGreenNodeId = incomingNeighbors[greenIdx];
                    node->incomingLights[node->currentGreenNodeId] = TrafficLightState::GREEN;
                }
            } else {
                for (int neighborId : incomingNeighbors) {
                    node->incomingLights[neighborId] = TrafficLightState::OFF;
                }
            }
        }
    }
}
