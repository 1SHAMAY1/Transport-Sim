#include "VehicleManager.hpp"
#include <imgui.h>
#include <random>

namespace TransportSim {

VehicleManager::VehicleManager() : m_rng(std::random_device{}()) {}

void VehicleManager::SpawnRandomVehicles(int numCars, int numTrains, int numPlanes, int numShips) {
    std::uniform_real_distribution<float> posDist(0.0f, 100.0f);
    std::uniform_real_distribution<float> fuelDist(50.0f, 100.0f);
    std::uniform_real_distribution<float> speedDist(10.0f, 40.0f);
    for (int i = 0; i < numCars; ++i) {
        Car car("Car" + std::to_string(i), fuelDist(m_rng), speedDist(m_rng));
        Path path;
        for (int j = 0; j < 5; ++j) path.waypoints.push_back({posDist(m_rng), posDist(m_rng)});
        car.setPath(path);
        m_cars.push_back(car);
    }
    for (int i = 0; i < numTrains; ++i) {
        Train train("Train" + std::to_string(i), fuelDist(m_rng), speedDist(m_rng));
        Path path;
        for (int j = 0; j < 3; ++j) path.waypoints.push_back({posDist(m_rng), posDist(m_rng)});
        train.setPath(path);
        m_trains.push_back(train);
    }
    for (int i = 0; i < numPlanes; ++i) {
        Plane plane("Plane" + std::to_string(i), fuelDist(m_rng), speedDist(m_rng) + 60.0f);
        Path path;
        for (int j = 0; j < 7; ++j) path.waypoints.push_back({posDist(m_rng), posDist(m_rng)});
        plane.setPath(path);
        m_planes.push_back(plane);
    }
    for (int i = 0; i < numShips; ++i) {
        Ship ship("Ship" + std::to_string(i), fuelDist(m_rng), speedDist(m_rng));
        Path path;
        for (int j = 0; j < 4; ++j) path.waypoints.push_back({posDist(m_rng), posDist(m_rng)});
        ship.setPath(path);
        m_ships.push_back(ship);
    }
}

void VehicleManager::UpdateAll(float dt) {
    for (auto& car : m_cars) car.move(dt);
    for (auto& train : m_trains) train.move(dt);
    for (auto& plane : m_planes) plane.move(dt);
    for (auto& ship : m_ships) ship.move(dt);
}

void VehicleManager::SelectVehicle(VehicleType type, size_t index) {
    m_selected = {type, index, true};
}
void VehicleManager::DeselectVehicle() {
    m_selected = {VehicleType::Car, 0, false};
}
VehicleManager::SelectedVehicle VehicleManager::getSelectedVehicle() const {
    return m_selected;
}

void VehicleManager::RenderAll() {
    ImGui::Begin("Vehicles");
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 base = ImGui::GetCursorScreenPos();
    float y = 0.0f;
    float scale = 1.0f;
    size_t idx = 0;
    // Cars
    ImGui::Text("Cars: %d", (int)m_cars.size());
    for (const auto& car : m_cars) {
        ImVec2 pos = ImVec2(base.x + 10, base.y + y);
        bool selected = m_selected.valid && m_selected.type == VehicleType::Car && m_selected.index == idx;
        car.RenderVehicle(pos, scale, selected);
        ImGui::SetCursorScreenPos(ImVec2(base.x, base.y + y + 30));
        ImGui::InvisibleButton(("car_btn_" + std::to_string(idx)).c_str(), ImVec2(60, 30));
        if (ImGui::IsItemClicked()) SelectVehicle(VehicleType::Car, idx);
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            SelectVehicle(VehicleType::Car, idx);
            ImGui::OpenPopup("vehicle_context");
        }
        y += 40;
        ++idx;
    }
    idx = 0;
    ImGui::Separator();
    ImGui::Text("Trains: %d", (int)m_trains.size());
    for (const auto& train : m_trains) {
        ImVec2 pos = ImVec2(base.x + 10, base.y + y);
        bool selected = m_selected.valid && m_selected.type == VehicleType::Train && m_selected.index == idx;
        train.RenderVehicle(pos, scale, selected);
        ImGui::SetCursorScreenPos(ImVec2(base.x, base.y + y + 30));
        ImGui::InvisibleButton(("train_btn_" + std::to_string(idx)).c_str(), ImVec2(60, 30));
        if (ImGui::IsItemClicked()) SelectVehicle(VehicleType::Train, idx);
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            SelectVehicle(VehicleType::Train, idx);
            ImGui::OpenPopup("vehicle_context");
        }
        y += 40;
        ++idx;
    }
    idx = 0;
    ImGui::Separator();
    ImGui::Text("Planes: %d", (int)m_planes.size());
    for (const auto& plane : m_planes) {
        ImVec2 pos = ImVec2(base.x + 10, base.y + y);
        bool selected = m_selected.valid && m_selected.type == VehicleType::Plane && m_selected.index == idx;
        plane.RenderVehicle(pos, scale, selected);
        ImGui::SetCursorScreenPos(ImVec2(base.x, base.y + y + 30));
        ImGui::InvisibleButton(("plane_btn_" + std::to_string(idx)).c_str(), ImVec2(60, 30));
        if (ImGui::IsItemClicked()) SelectVehicle(VehicleType::Plane, idx);
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            SelectVehicle(VehicleType::Plane, idx);
            ImGui::OpenPopup("vehicle_context");
        }
        y += 40;
        ++idx;
    }
    idx = 0;
    ImGui::Separator();
    ImGui::Text("Ships: %d", (int)m_ships.size());
    for (const auto& ship : m_ships) {
        ImVec2 pos = ImVec2(base.x + 10, base.y + y);
        bool selected = m_selected.valid && m_selected.type == VehicleType::Ship && m_selected.index == idx;
        ship.RenderVehicle(pos, scale, selected);
        ImGui::SetCursorScreenPos(ImVec2(base.x, base.y + y + 30));
        ImGui::InvisibleButton(("ship_btn_" + std::to_string(idx)).c_str(), ImVec2(60, 30));
        if (ImGui::IsItemClicked()) SelectVehicle(VehicleType::Ship, idx);
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            SelectVehicle(VehicleType::Ship, idx);
            ImGui::OpenPopup("vehicle_context");
        }
        y += 40;
        ++idx;
    }
    // Context menu
    if (ImGui::BeginPopup("vehicle_context")) {
        if (ImGui::MenuItem("Deselect")) DeselectVehicle();
        // More context actions can be added here
        ImGui::EndPopup();
    }
    ImGui::End();
}

const std::vector<Car>& VehicleManager::getCars() const { return m_cars; }
const std::vector<Train>& VehicleManager::getTrains() const { return m_trains; }
const std::vector<Plane>& VehicleManager::getPlanes() const { return m_planes; }
const std::vector<Ship>& VehicleManager::getShips() const { return m_ships; }

std::vector<Car>& VehicleManager::getCars() { return m_cars; }
std::vector<Train>& VehicleManager::getTrains() { return m_trains; }
std::vector<Plane>& VehicleManager::getPlanes() { return m_planes; }
std::vector<Ship>& VehicleManager::getShips() { return m_ships; }

} // namespace TransportSim
