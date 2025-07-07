#include "Simulation.hpp"
#include <imgui.h>
#include <random>
#include <fstream>

namespace TransportSim {

Simulation::Simulation() : m_rng(std::random_device{}()) {}

void Simulation::spawnVehicles(int cars, int trains, int planes, int ships) {
    m_vehicleManager.SpawnRandomVehicles(cars, trains, planes, ships);
}

void Simulation::update(float dt) {
    updateObstructions();
    m_vehicleManager.UpdateAll(dt);
}

void Simulation::render() {
    ImGui::Begin("Simulation State");
    ImGui::Text("Simulation running...");
    ImGui::End();
    m_vehicleManager.RenderAll();
}

VehicleManager& Simulation::getVehicleManager() {
    return m_vehicleManager;
}

void Simulation::updateObstructions() {
    // Randomly obstruct some waypoints for demo
    std::uniform_int_distribution<int> obstructDist(0, 100);
    for (auto& car : m_vehicleManager.getCars()) {
        auto& path = car.getPath();
        for (auto& wp : path.waypoints) {
            wp.obstructed = (obstructDist(m_rng) < 2); // ~2% chance
        }
    }
    for (auto& train : m_vehicleManager.getTrains()) {
        auto& path = train.getPath();
        for (auto& wp : path.waypoints) {
            wp.obstructed = (obstructDist(m_rng) < 2);
        }
    }
    for (auto& plane : m_vehicleManager.getPlanes()) {
        auto& path = plane.getPath();
        for (auto& wp : path.waypoints) {
            wp.obstructed = (obstructDist(m_rng) < 1); // Planes less likely
        }
    }
    for (auto& ship : m_vehicleManager.getShips()) {
        auto& path = ship.getPath();
        for (auto& wp : path.waypoints) {
            wp.obstructed = (obstructDist(m_rng) < 3); // Ships more likely
        }
    }
}

void Simulation::SaveState(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) return;
    // Save cars
    out << "Cars " << m_vehicleManager.getCars().size() << "\n";
    for (const auto& car : m_vehicleManager.getCars()) {
        out << car.getName() << ' ' << car.getFuel() << ' ' << car.getSpeed() << ' ' << car.getPath().waypoints.size();
        for (const auto& wp : car.getPath().waypoints) {
            out << ' ' << wp.x << ' ' << wp.y << ' ' << wp.obstructed;
        }
        out << '\n';
    }
    // Save trains
    out << "Trains " << m_vehicleManager.getTrains().size() << "\n";
    for (const auto& train : m_vehicleManager.getTrains()) {
        out << train.getName() << ' ' << train.getFuel() << ' ' << train.getSpeed() << ' ' << train.getPath().waypoints.size();
        for (const auto& wp : train.getPath().waypoints) {
            out << ' ' << wp.x << ' ' << wp.y << ' ' << wp.obstructed;
        }
        out << '\n';
    }
    // Save planes
    out << "Planes " << m_vehicleManager.getPlanes().size() << "\n";
    for (const auto& plane : m_vehicleManager.getPlanes()) {
        out << plane.getName() << ' ' << plane.getFuel() << ' ' << plane.getSpeed() << ' ' << plane.getPath().waypoints.size();
        for (const auto& wp : plane.getPath().waypoints) {
            out << ' ' << wp.x << ' ' << wp.y << ' ' << wp.obstructed;
        }
        out << '\n';
    }
    // Save ships
    out << "Ships " << m_vehicleManager.getShips().size() << "\n";
    for (const auto& ship : m_vehicleManager.getShips()) {
        out << ship.getName() << ' ' << ship.getFuel() << ' ' << ship.getSpeed() << ' ' << ship.getPath().waypoints.size();
        for (const auto& wp : ship.getPath().waypoints) {
            out << ' ' << wp.x << ' ' << wp.y << ' ' << wp.obstructed;
        }
        out << '\n';
    }
}

void Simulation::LoadState(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return;
    m_vehicleManager.getCars().clear();
    m_vehicleManager.getTrains().clear();
    m_vehicleManager.getPlanes().clear();
    m_vehicleManager.getShips().clear();
    std::string type;
    size_t count;
    while (in >> type >> count) {
        for (size_t i = 0; i < count; ++i) {
            std::string name;
            float fuel, speed;
            size_t wpCount;
            in >> name >> fuel >> speed >> wpCount;
            Path path;
            for (size_t j = 0; j < wpCount; ++j) {
                float x, y;
                int obstructed;
                in >> x >> y >> obstructed;
                path.waypoints.push_back({x, y, (bool)obstructed});
            }
            if (type == "Cars") {
                Car car(name, fuel, speed);
                car.setPath(path);
                m_vehicleManager.getCars().push_back(car);
            } else if (type == "Trains") {
                Train train(name, fuel, speed);
                train.setPath(path);
                m_vehicleManager.getTrains().push_back(train);
            } else if (type == "Planes") {
                Plane plane(name, fuel, speed);
                plane.setPath(path);
                m_vehicleManager.getPlanes().push_back(plane);
            } else if (type == "Ships") {
                Ship ship(name, fuel, speed);
                ship.setPath(path);
                m_vehicleManager.getShips().push_back(ship);
            }
        }
    }
}

} // namespace TransportSim
