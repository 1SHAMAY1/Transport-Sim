#include "UI.hpp"
#include <imgui.h>

namespace TransportSim {

UI::UI() = default;

void UI::renderTopBar() {
    if (ImGui::BeginMainMenuBar()) {
        ImGui::Text("Transport-Sim");
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 60);
        if (ImGui::BeginMenu("...")) {
            if (ImGui::MenuItem("Enable profiling")) {
                if (m_onProfiling) m_onProfiling();
            }
            if (ImGui::MenuItem("View memory usage")) {
                if (m_onMemoryUsage) m_onMemoryUsage();
            }
            if (ImGui::MenuItem("Change vehicle path")) {
                if (m_onChangePath) m_onChangePath();
            }
            if (ImGui::MenuItem("Set destination")) {
                if (m_onSetDestination) m_onSetDestination();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Export Simulation")) {
                if (m_onExport) m_onExport();
            }
            if (ImGui::MenuItem("Import Simulation")) {
                if (m_onImport) m_onImport();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void UI::setProfilingCallback(const std::function<void()>& cb) {
    m_onProfiling = cb;
}
void UI::setMemoryUsageCallback(const std::function<void()>& cb) {
    m_onMemoryUsage = cb;
}
void UI::setChangePathCallback(const std::function<void()>& cb) {
    m_onChangePath = cb;
}
void UI::setSetDestinationCallback(const std::function<void()>& cb) {
    m_onSetDestination = cb;
}
void UI::setExportCallback(const std::function<void()>& cb) {
    m_onExport = cb;
}
void UI::setImportCallback(const std::function<void()>& cb) {
    m_onImport = cb;
}

} // namespace TransportSim
