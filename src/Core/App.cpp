#include "App.hpp"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <chrono>

namespace TransportSim {

App::App()
    : m_window(nullptr), m_running(true), m_inSimulation(false) {
    initWindow();
    initImGui();
    m_introScreen = std::make_unique<IntroScreen>();
    m_ui = std::make_unique<UI>();
    m_profiling = std::make_unique<Profiling>();
    m_simulation = std::make_unique<Simulation>();
    // Hook profiling window to UI menu
    m_ui->setProfilingCallback([this]() { m_profiling->setVisible(!m_profiling->isVisible()); });
    m_ui->setExportCallback([this]() { m_simulation->SaveState("sim_state.txt"); });
    m_ui->setImportCallback([this]() { m_simulation->LoadState("sim_state.txt"); });
}

App::~App() {
    cleanup();
}

void App::initWindow() {
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(1280, 720, "Transport-Sim", nullptr, nullptr);
    if (!m_window) throw std::runtime_error("Failed to create GLFW window");
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    if (gladLoadGL((GLADapiproc (*)(const char *))glfwGetProcAddress) == 0) throw std::runtime_error("Failed to initialize GLAD");
}

void App::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void App::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void App::run() {
    m_simulation->spawnVehicles(5, 2, 2, 2);
    mainLoop();
}

void App::mainLoop() {
    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();
    while (m_running && !glfwWindowShouldClose(m_window)) {
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        processInput();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        m_ui->renderTopBar();
        if (!m_inSimulation) {
            m_introScreen->update();
            m_introScreen->render();
            if (m_introScreen->isDone()) m_inSimulation = true;
        } else {
            m_simulation->update(dt);
            m_simulation->render();
            // Handle vehicle selection actions
            auto& vm = m_simulation->getVehicleManager();
            auto sel = vm.getSelectedVehicle();
            if (sel.valid) {
                // Keyboard rename
                static std::string lastName;
                static char nameBuf[32] = "";
                std::string currentName;
                if (sel.type == VehicleType::Car && sel.index < vm.getCars().size())
                    currentName = vm.getCars()[sel.index].getName();
                else if (sel.type == VehicleType::Train && sel.index < vm.getTrains().size())
                    currentName = vm.getTrains()[sel.index].getName();
                else if (sel.type == VehicleType::Plane && sel.index < vm.getPlanes().size())
                    currentName = vm.getPlanes()[sel.index].getName();
                else if (sel.type == VehicleType::Ship && sel.index < vm.getShips().size())
                    currentName = vm.getShips()[sel.index].getName();

                if (lastName != currentName) {
                    strncpy(nameBuf, currentName.c_str(), sizeof(nameBuf));
                    nameBuf[sizeof(nameBuf)-1] = '\0';
                    lastName = currentName;
                }

                if (ImGui::Begin("Vehicle Actions")) {
                    if (ImGui::InputText("Rename", nameBuf, sizeof(nameBuf))) {
                        if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                            if (sel.type == VehicleType::Car && sel.index < vm.getCars().size())
                                vm.getCars()[sel.index].setName(nameBuf);
                            else if (sel.type == VehicleType::Train && sel.index < vm.getTrains().size())
                                vm.getTrains()[sel.index].setName(nameBuf);
                            else if (sel.type == VehicleType::Plane && sel.index < vm.getPlanes().size())
                                vm.getPlanes()[sel.index].setName(nameBuf);
                            else if (sel.type == VehicleType::Ship && sel.index < vm.getShips().size())
                                vm.getShips()[sel.index].setName(nameBuf);
                            lastName = nameBuf;
                        }
                    }
                    // Change type (for demo, cycle type)
                    if (ImGui::Button("Change Type")) {
                        if (sel.type == VehicleType::Car && sel.index < vm.getCars().size()) {
                            auto v = vm.getCars()[sel.index];
                            vm.getCars().erase(vm.getCars().begin() + sel.index);
                            vm.getPlanes().push_back(Plane(v.getName(), v.getFuel(), v.getSpeed()));
                        } else if (sel.type == VehicleType::Plane && sel.index < vm.getPlanes().size()) {
                            auto v = vm.getPlanes()[sel.index];
                            vm.getPlanes().erase(vm.getPlanes().begin() + sel.index);
                            vm.getTrains().push_back(Train(v.getName(), v.getFuel(), v.getSpeed()));
                        } else if (sel.type == VehicleType::Train && sel.index < vm.getTrains().size()) {
                            auto v = vm.getTrains()[sel.index];
                            vm.getTrains().erase(vm.getTrains().begin() + sel.index);
                            vm.getShips().push_back(Ship(v.getName(), v.getFuel(), v.getSpeed()));
                        } else if (sel.type == VehicleType::Ship && sel.index < vm.getShips().size()) {
                            auto v = vm.getShips()[sel.index];
                            vm.getShips().erase(vm.getShips().begin() + sel.index);
                            vm.getCars().push_back(Car(v.getName(), v.getFuel(), v.getSpeed()));
                        }
                        vm.DeselectVehicle();
                    }
                    ImGui::End();
                }
                // Right-click to set destination (popup for demo)
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    ImGui::OpenPopup("set_dest_popup");
                }
                if (ImGui::BeginPopup("set_dest_popup")) {
                    ImGui::Text("Set new destination (demo)");
                    if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
            }
        }
        m_profiling->render();
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void App::processInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) m_running = false;
}

} // namespace TransportSim
