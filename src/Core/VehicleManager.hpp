#pragma once
#include "Vehicle.hpp"
#include <vector>
#include <random>

namespace TransportSim {

class VehicleManager {
public:
    VehicleManager();
    void SpawnRandomVehicles(int numCars, int numTrains, int numPlanes, int numShips);
    void UpdateAll(float dt);
    void RenderAll();
    // Expose access to vehicles for simulation
    std::vector<Car>& getCars();
    std::vector<Train>& getTrains();
    std::vector<Plane>& getPlanes();
    std::vector<Ship>& getShips();
    const std::vector<Car>& getCars() const;
    const std::vector<Train>& getTrains() const;
    const std::vector<Plane>& getPlanes() const;
    const std::vector<Ship>& getShips() const;
    // Selection logic
    struct SelectedVehicle {
        VehicleType type;
        size_t index;
        bool valid = false;
    };
    void SelectVehicle(VehicleType type, size_t index);
    void DeselectVehicle();
    SelectedVehicle getSelectedVehicle() const;
private:
    std::vector<Car> m_cars;
    std::vector<Train> m_trains;
    std::vector<Plane> m_planes;
    std::vector<Ship> m_ships;
    std::mt19937 m_rng;
    SelectedVehicle m_selected;
};

} // namespace TransportSim
