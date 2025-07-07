#pragma once
#include "VehicleManager.hpp"
#include <random>

namespace TransportSim {

class Simulation {
public:
    Simulation();
    void update(float dt);
    void render();
    void spawnVehicles(int cars, int trains, int planes, int ships);
    VehicleManager& getVehicleManager();
    // State persistence
    void SaveState(const std::string& filename) const;
    void LoadState(const std::string& filename);
private:
    VehicleManager m_vehicleManager;
    std::mt19937 m_rng;
    void updateObstructions();
};

} // namespace TransportSim
