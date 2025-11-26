#pragma once
#include <memory>

class Camera;
class TransportSimulation;
class Shader;

class Application {
public:
    Application();
    ~Application();

    void Run();
    
private:
    void Update(float deltaTime);
    void Render();
    void RenderGrid();
    void RenderVehicles();
    void RenderUI();
    
    std::shared_ptr<Camera> m_Camera;
    std::shared_ptr<TransportSimulation> m_Simulation;
    std::shared_ptr<Shader> m_Shader;
    
    unsigned int m_CubeVAO = 0;
    unsigned int m_CubeVBO = 0;
    unsigned int m_LineVAO = 0;
    unsigned int m_LineVBO = 0;
    
    float m_LastFrameTime = 0.0f;
};
