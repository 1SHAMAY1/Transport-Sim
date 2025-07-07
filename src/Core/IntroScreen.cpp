#include "IntroScreen.hpp"
#include <imgui.h>
#include <cmath>

namespace TransportSim {

IntroScreen::IntroScreen()
    : m_startTime(std::chrono::steady_clock::now()), m_animProgress(0.0f), m_showButton(false), m_startClicked(false) {}

void IntroScreen::update() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    float elapsed = duration<float>(now - m_startTime).count();
    m_animProgress = std::min(elapsed / 2.0f, 1.0f); // 2 seconds animation
    if (m_animProgress >= 1.0f) m_showButton = true;
}

void IntroScreen::render() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("##IntroScreen", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    float alpha = 0.5f + 0.5f * std::sin(m_animProgress * 3.1415f); // Simple fade in
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.4f);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 120);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    ImGui::Text("Welcome to Transport-Sim!");
    ImGui::PopStyleVar();
    if (m_showButton) {
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.6f);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 80);
        if (ImGui::Button("Start Simulation", ImVec2(160, 40))) {
            m_startClicked = true;
        }
    }
    ImGui::End();
}

bool IntroScreen::isDone() const {
    return m_showButton && m_startClicked;
}

bool IntroScreen::startClicked() const {
    return m_startClicked;
}

} // namespace TransportSim
