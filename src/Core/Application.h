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
    
    static void OnResize(struct GLFWwindow* window, int width, int height);
    
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
    
    // Batch Rendering Members
    unsigned int m_BatchNodesVAO = 0;
    unsigned int m_BatchNodesVBO = 0;
    int m_BatchNodesCount = 0;
    
    unsigned int m_BatchOneWayVAO = 0;
    unsigned int m_BatchOneWayVBO = 0;
    int m_BatchOneWayCount = 0;
    
    unsigned int m_BatchTwoWayVAO = 0;
    unsigned int m_BatchTwoWayVBO = 0;
    int m_BatchTwoWayCount = 0;
    
    unsigned int m_BatchLightsVAO = 0;
    unsigned int m_BatchLightsVBO = 0;
    
    void BuildGridMesh();
    void RenderGridBatched();
    
    float m_LastFrameTime = 0.0f;
};
