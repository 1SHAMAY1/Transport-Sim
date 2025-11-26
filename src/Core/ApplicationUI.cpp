
void Application::RenderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Main stats window
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Transport Simulator - Stats", nullptr, ImGuiWindowFlags_NoCollapse);
    
    ImGui::Text("3D Transport Simulator");
    ImGui::Separator();
    
    // Network info
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "Network Information");
    auto graph = m_Simulation->GetGraph();
    ImGui::Text("Total Nodes: %zu", graph->GetNodeCount());
    ImGui::Text("Grid Size: 8x8 (Single Level)");
    
    // Count edges
    int totalEdges = 0;
    int oneWayEdges = 0;
    int twoWayEdges = 0;
    
    for (const auto& [id, node] : graph->GetNodes()) {
        for (const auto& edge : node->edges) {
            totalEdges++;
            
            bool isBidirectional = false;
            for (const auto& reverseEdge : edge->to->edges) {
                if (reverseEdge->to->id == node->id) {
                    isBidirectional = true;
                    break;
                }
            }
            
            if (isBidirectional) {
                twoWayEdges++;
            } else {
                oneWayEdges++;
            }
        }
    }
    
    ImGui::Text("Total Roads: %d", totalEdges);
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 0.5f, 1.0f), "  Two-Way: %d", twoWayEdges / 2);  // Divide by 2 since counted both directions
    ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.5f, 1.0f), "  One-Way: %d", oneWayEdges);
    
    ImGui::Separator();
    
    // Vehicle info
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.4f, 1.0f), "Vehicle Information");
    const auto& vehicles = m_Simulation->GetVehicles();
    ImGui::Text("Active Vehicles: %zu", vehicles.size());
    
    int movingVehicles = 0;
    int stoppedVehicles = 0;
    
    for (const auto& vehicle : vehicles) {
        if (vehicle->IsMoving()) {
            movingVehicles++;
        } else {
            stoppedVehicles++;
        }
    }
    
    ImGui::Text("  Moving: %d", movingVehicles);
    ImGui::Text("  Stopped: %d", stoppedVehicles);
    
    ImGui::Separator();
    
    // Performance
    ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.8f, 1.0f), "Performance");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    
    ImGui::Separator();
    
    // Features
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.8f, 1.0f), "Features");
    ImGui::BulletText("A* Pathfinding");
    ImGui::BulletText("Collision Avoidance");
    ImGui::BulletText("One-Way Roads");
    ImGui::BulletText("Diagonal Shortcuts");
    ImGui::BulletText("Pentagon Nodes");
    ImGui::BulletText("Dynamic Vehicle Rotation");
    
    ImGui::End();
    
    // Vehicle details window (compact)
    ImGui::SetNextWindowPos(ImVec2(10, 420), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 290), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Vehicle Details", nullptr, ImGuiWindowFlags_NoCollapse);
    
    ImGui::Text("Live Vehicle Status (Top 5)");
    ImGui::Separator();
    
    // Show first 5 vehicles
    for (size_t i = 0; i < std::min(size_t(5), vehicles.size()); i++) {
        const auto& vehicle = vehicles[i];
        
        float hue = (i * 60.0f) / 360.0f;
        ImVec4 color(
            0.5f + 0.5f * sin(hue * 6.28f),
            0.5f + 0.5f * sin((hue + 0.33f) * 6.28f),
            0.5f + 0.5f * sin((hue + 0.67f) * 6.28f),
            1.0f
        );
        
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("Vehicle %d:", vehicle->GetId());
        ImGui::PopStyleColor();
        
        ImGui::Indent();
        ImGui::Text("Speed: %.1f units/s", vehicle->GetSpeed());
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", 
                    vehicle->GetPosition().x,
                    vehicle->GetPosition().y,
                    vehicle->GetPosition().z);
        ImGui::Text("Status: %s", vehicle->IsMoving() ? "Moving" : "Stopped");
        ImGui::Unindent();
        ImGui::Separator();
    }
    
    ImGui::End();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
