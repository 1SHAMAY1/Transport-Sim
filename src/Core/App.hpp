#pragma once

#include "IntroScreen.hpp"
#include "UI.hpp"
#include "Profiling.hpp"
#include "Simulation.hpp"
#include <memory>

struct GLFWwindow;

namespace TransportSim {

class App {
public:
    App();
    ~App();
    void run();
private:
    void initWindow();
    void initImGui();
    void cleanup();
    void mainLoop();
    void render();
    void processInput();

    GLFWwindow* m_window;
    std::unique_ptr<IntroScreen> m_introScreen;
    std::unique_ptr<UI> m_ui;
    std::unique_ptr<Profiling> m_profiling;
    std::unique_ptr<Simulation> m_simulation;
    bool m_running;
    bool m_inSimulation;
};

} // namespace TransportSim
