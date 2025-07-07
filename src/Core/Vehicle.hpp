#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <optional>
#include "imgui.h"

namespace TransportSim {

struct Waypoint {
    float x, y;
    bool obstructed = false;
};

struct Path {
    std::vector<Waypoint> waypoints;
    size_t current = 0;
    bool isObstructed() const {
        if (current < waypoints.size())
            return waypoints[current].obstructed;
        return false;
    }
};

// VehicleType enum for clarity
enum class VehicleType { Car, Train, Plane, Ship };

// Base template Vehicle class
// Specialize if needed for different vehicle types

template <VehicleType T>
class Vehicle {
public:
    Vehicle(const std::string& name, float maxFuel, float speed);
    void move(float dt); // Move along path, dt = delta time
    void refill();
    void setPath(const Path& path);
    void setSpeed(float s);
    float getFuel() const;
    float getSpeed() const;
    const std::string& getName() const;
    Path& getPath();
    const Path& getPath() const;
    // ImGui rendering
    void RenderVehicle(const ImVec2& pos, float scale, bool selected) const;
    void setName(const std::string& name);
protected:
    std::string m_name;
    float m_fuel;
    float m_maxFuel;
    float m_speed; // units per second
    Path m_path;
};

// Type aliases for convenience
using Car   = Vehicle<VehicleType::Car>;
using Train = Vehicle<VehicleType::Train>;
using Plane = Vehicle<VehicleType::Plane>;
using Ship  = Vehicle<VehicleType::Ship>;

extern template class Vehicle<VehicleType::Car>;
extern template class Vehicle<VehicleType::Train>;
extern template class Vehicle<VehicleType::Plane>;
extern template class Vehicle<VehicleType::Ship>;

} // namespace TransportSim
