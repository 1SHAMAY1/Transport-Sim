#include "Vehicle.hpp"
#include <algorithm>
#include "imgui.h"

namespace TransportSim {

template <VehicleType T>
Vehicle<T>::Vehicle(const std::string& name, float maxFuel, float speed)
    : m_name(name), m_fuel(maxFuel), m_maxFuel(maxFuel), m_speed(speed) {}

template <VehicleType T>
void Vehicle<T>::move(float dt) {
    if (m_fuel <= 0.0f || m_path.current >= m_path.waypoints.size() || m_path.isObstructed())
        return;
    m_fuel -= m_speed * dt * 0.01f;
    if (m_fuel < 0.0f) m_fuel = 0.0f;
    if (m_fuel > 0.0f) {
        m_path.current = std::min(m_path.current + 1, m_path.waypoints.size());
    }
}

template <VehicleType T>
void Vehicle<T>::refill() {
    m_fuel = m_maxFuel;
}

template <VehicleType T>
void Vehicle<T>::setPath(const Path& path) {
    m_path = path;
    m_path.current = 0;
}

template <VehicleType T>
void Vehicle<T>::setSpeed(float s) {
    m_speed = s;
}

template <VehicleType T>
float Vehicle<T>::getFuel() const {
    return m_fuel;
}

template <VehicleType T>
float Vehicle<T>::getSpeed() const {
    return m_speed;
}

template <VehicleType T>
const std::string& Vehicle<T>::getName() const {
    return m_name;
}

template <VehicleType T>
Path& Vehicle<T>::getPath() {
    return m_path;
}

template <VehicleType T>
const Path& Vehicle<T>::getPath() const {
    return m_path;
}

template <VehicleType T>
void Vehicle<T>::setName(const std::string& name) {
    m_name = name;
}

template <VehicleType T>
void Vehicle<T>::RenderVehicle(const ImVec2& pos, float scale, bool selected) const {
    ImU32 color;
    if constexpr (T == VehicleType::Car) color = IM_COL32(40, 120, 255, 255);
    else if constexpr (T == VehicleType::Train) color = IM_COL32(60, 200, 60, 255);
    else if constexpr (T == VehicleType::Plane) color = IM_COL32(220, 60, 60, 255);
    else if constexpr (T == VehicleType::Ship) color = IM_COL32(255, 140, 40, 255);
    else color = IM_COL32(200, 200, 200, 255);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 rect_min = pos;
    ImVec2 rect_max = ImVec2(pos.x + 40 * scale, pos.y + 20 * scale);
    draw_list->AddRectFilled(rect_min, rect_max, color, 4.0f);
    if (selected) draw_list->AddRect(rect_min, rect_max, IM_COL32(255,255,0,255), 4.0f, 0, 3.0f);
    float fuelFrac = m_fuel / m_maxFuel;
    ImVec2 fuel_min = ImVec2(rect_min.x, rect_max.y + 2);
    ImVec2 fuel_max = ImVec2(rect_min.x + 40 * scale * fuelFrac, rect_max.y + 8);
    draw_list->AddRectFilled(fuel_min, ImVec2(rect_min.x + 40 * scale, rect_max.y + 8), IM_COL32(60,60,60,255), 2.0f);
    draw_list->AddRectFilled(fuel_min, fuel_max, IM_COL32(80,220,80,255), 2.0f);
    if (m_path.isObstructed()) {
        draw_list->AddText(ImVec2(rect_min.x, rect_max.y + 12), IM_COL32(255,80,80,255), "DELAY");
    }
}

// Explicit template instantiations

template class Vehicle<VehicleType::Car>;
template class Vehicle<VehicleType::Train>;
template class Vehicle<VehicleType::Plane>;
template class Vehicle<VehicleType::Ship>;

} // namespace TransportSim
