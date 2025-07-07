#include "Profiling.hpp"
#include <imgui.h>
#include <random>

namespace TransportSim {

Profiling::Profiling()
    : m_visible(false), m_cpuUsage(0.0f), m_gpuUsage(0.0f), m_memUsage(0.0f) {}

void Profiling::render() {
    if (!m_visible) return;
    // Simulate dummy stats
    static std::default_random_engine rng;
    static std::uniform_real_distribution<float> cpuDist(10.0f, 80.0f);
    static std::uniform_real_distribution<float> gpuDist(5.0f, 70.0f);
    static std::uniform_real_distribution<float> memDist(100.0f, 800.0f);
    m_cpuUsage = cpuDist(rng);
    m_gpuUsage = gpuDist(rng);
    m_memUsage = memDist(rng);

    ImGui::Begin("Profiling", &m_visible, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("CPU Usage: %.1f%%", m_cpuUsage);
    ImGui::ProgressBar(m_cpuUsage / 100.0f, ImVec2(200, 0));
    ImGui::Text("GPU Usage: %.1f%%", m_gpuUsage);
    ImGui::ProgressBar(m_gpuUsage / 100.0f, ImVec2(200, 0));
    ImGui::Text("Memory Usage: %.1f MB", m_memUsage);
    ImGui::ProgressBar(m_memUsage / 1000.0f, ImVec2(200, 0));
    ImGui::End();
}

void Profiling::setVisible(bool v) {
    m_visible = v;
}

bool Profiling::isVisible() const {
    return m_visible;
}

} // namespace TransportSim
