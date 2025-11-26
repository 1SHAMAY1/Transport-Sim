#include "Application.h"
#include "../Renderer/Camera.h"
#include "../Renderer/Shader.h"
#include "../Simulation/TransportSimulation.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

static GLFWwindow* s_Window = nullptr;

static const char* vertexShaderSource = R"(
    #version 460 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 u_Model;
    uniform mat4 u_View;
    uniform mat4 u_Projection;
    void main() {
        gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
    }
)";

static const char* fragmentShaderSource = R"(
    #version 460 core
    out vec4 FragColor;
    uniform vec4 u_Color;
    void main() {
        FragColor = u_Color;
    }
)";

Application::Application() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    s_Window = glfwCreateWindow(1280, 720, "Transport Simulator 3D - C++ DSA", NULL, NULL);
    if (!s_Window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(s_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
    
    glViewport(0, 0, 1280, 720);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(2.0f);
    
    m_Shader = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource);
    m_Camera = std::make_shared<Camera>(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
    m_Camera->SetPosition(glm::vec3(20.0f, 30.0f, 40.0f));
    m_Camera->SetRotation(-30.0f, -135.0f);
    
    m_Simulation = std::make_shared<TransportSimulation>();
    m_Simulation->Initialize();
    
    float cubeVertices[] = {
        -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f
    };
    
    glGenVertexArrays(1, &m_CubeVAO);
    glGenBuffers(1, &m_CubeVBO);
    glBindVertexArray(m_CubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glGenVertexArrays(1, &m_LineVAO);
    glGenBuffers(1, &m_LineVBO);
    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(s_Window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    
    std::cout << "Initialization complete!" << std::endl;
    std::cout << "Controls: SPACE = Toggle Auto/Manual Camera" << std::endl;
    std::cout << "  Manual: WASD = Move, QE = Up/Down, Arrows = Rotate" << std::endl;
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glDeleteVertexArrays(1, &m_CubeVAO);
    glDeleteBuffers(1, &m_CubeVBO);
    glDeleteVertexArrays(1, &m_LineVAO);
    glDeleteBuffers(1, &m_LineVBO);
    
    if (s_Window) glfwDestroyWindow(s_Window);
    glfwTerminate();
}

void Application::Run() {
    while (s_Window && !glfwWindowShouldClose(s_Window)) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - m_LastFrameTime;
        m_LastFrameTime = currentTime;
        
        Update(deltaTime);
        Render();
        
        glfwSwapBuffers(s_Window);
        glfwPollEvents();
    }
}

void Application::Update(float deltaTime) {
    static bool manualControl = false;
    static float manualYaw = -45.0f;
    static float manualPitch = -30.0f;
    static glm::vec3 manualCameraPos(55.0f, 45.0f, 55.0f);
    static bool spacePressed = false;
    
    if (glfwGetKey(s_Window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!spacePressed) {
            manualControl = !manualControl;
            spacePressed = true;
            std::cout << (manualControl ? "Manual camera" : "Auto camera") << std::endl;
        }
    } else {
        spacePressed = false;
    }
    
    if (manualControl) {
        float cameraSpeed = 20.0f * deltaTime;
        if (glfwGetKey(s_Window, GLFW_KEY_W) == GLFW_PRESS) manualCameraPos.z -= cameraSpeed;
        if (glfwGetKey(s_Window, GLFW_KEY_S) == GLFW_PRESS) manualCameraPos.z += cameraSpeed;
        if (glfwGetKey(s_Window, GLFW_KEY_A) == GLFW_PRESS) manualCameraPos.x -= cameraSpeed;
        if (glfwGetKey(s_Window, GLFW_KEY_D) == GLFW_PRESS) manualCameraPos.x += cameraSpeed;
        if (glfwGetKey(s_Window, GLFW_KEY_Q) == GLFW_PRESS) manualCameraPos.y -= cameraSpeed;
        if (glfwGetKey(s_Window, GLFW_KEY_E) == GLFW_PRESS) manualCameraPos.y += cameraSpeed;
        if (glfwGetKey(s_Window, GLFW_KEY_LEFT) == GLFW_PRESS) manualYaw -= 50.0f * deltaTime;
        if (glfwGetKey(s_Window, GLFW_KEY_RIGHT) == GLFW_PRESS) manualYaw += 50.0f * deltaTime;
        if (glfwGetKey(s_Window, GLFW_KEY_UP) == GLFW_PRESS) manualPitch += 50.0f * deltaTime;
        if (glfwGetKey(s_Window, GLFW_KEY_DOWN) == GLFW_PRESS) manualPitch -= 50.0f * deltaTime;
        
        m_Camera->SetPosition(manualCameraPos);
        m_Camera->SetRotation(manualPitch, manualYaw);
    } else {
        static float angle = 0.0f;
        angle += deltaTime * 10.0f;
        
        float radius = 120.0f;
        float height = 65.0f;
        glm::vec3 cameraPos(
            cos(glm::radians(angle)) * radius + 55.0f,
            height,
            sin(glm::radians(angle)) * radius + 55.0f
        );
        m_Camera->SetPosition(cameraPos);
        
        glm::vec3 center(55.0f, 0.0f, 55.0f);
        glm::vec3 direction = glm::normalize(center - cameraPos);
        
        float pitch = glm::degrees(asin(direction.y));
        float yaw = glm::degrees(atan2(direction.z, direction.x));
        m_Camera->SetRotation(pitch, yaw);
    }
    
    m_Camera->Update(deltaTime);
    m_Simulation->Update(deltaTime);
}

void Application::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    m_Shader->Bind();
    m_Shader->SetMat4("u_View", m_Camera->GetViewMatrix());
    m_Shader->SetMat4("u_Projection", m_Camera->GetProjectionMatrix());
    
    RenderGrid();
    RenderVehicles();
    
    m_Shader->Unbind();
    RenderUI();
}

void Application::RenderGrid() {
    auto graph = m_Simulation->GetGraph();
    
    // Render pentagon nodes
    for (const auto& [id, node] : graph->GetNodes()) {
        float pentagonVertices[] = {
             0.0f,  0.5f, 0.0f, -0.48f, 0.15f, 0.0f, -0.29f, -0.4f, 0.0f,
             0.29f, -0.4f, 0.0f, 0.48f,  0.15f, 0.0f
        };
        
        glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pentagonVertices), pentagonVertices, GL_DYNAMIC_DRAW);
        glBindVertexArray(m_LineVAO);
        
        float rotation = 0.0f;
        if (!node->edges.empty()) {
            glm::vec3 dir = glm::normalize(node->edges[0]->to->position - node->position);
            rotation = atan2(dir.x, dir.z);
        }
        
        m_Shader->SetFloat4("u_Color", glm::vec4(0.9f, 0.9f, 0.95f, 1.0f));
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, node->position + glm::vec3(0.0f, 0.1f, 0.0f));
        model = glm::rotate(model, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f, 1.0f, 0.8f));
        
        m_Shader->SetMat4("u_Model", model);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
    }
    
    // Render roads with parallel lines for two-way
    glBindVertexArray(m_LineVAO);
    
    for (const auto& [id, node] : graph->GetNodes()) {
        for (const auto& edge : node->edges) {
            bool isBidirectional = false;
            for (const auto& reverseEdge : edge->to->edges) {
                if (reverseEdge->to->id == node->id) {
                    isBidirectional = true;
                    break;
                }
            }
            
            if (isBidirectional) {
                // Two-way: Draw parallel green lines
                m_Shader->SetFloat4("u_Color", glm::vec4(0.4f, 0.5f, 0.4f, 1.0f));
                
                glm::vec3 roadDir = glm::normalize(edge->to->position - node->position);
                glm::vec3 perpendicular = glm::normalize(glm::cross(roadDir, glm::vec3(0.0f, 1.0f, 0.0f)));
                float offset = 0.2f;
                
                // Line 1
                glm::vec3 offset1 = perpendicular * offset;
                float lineVertices1[] = {
                    node->position.x + offset1.x, node->position.y, node->position.z + offset1.z,
                    edge->to->position.x + offset1.x, edge->to->position.y, edge->to->position.z + offset1.z
                };
                
                glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices1), lineVertices1, GL_DYNAMIC_DRAW);
                
                glm::mat4 model = glm::mat4(1.0f);
                m_Shader->SetMat4("u_Model", model);
                glDrawArrays(GL_LINES, 0, 2);
                
                // Line 2
                glm::vec3 offset2 = perpendicular * -offset;
                float lineVertices2[] = {
                    node->position.x + offset2.x, node->position.y, node->position.z + offset2.z,
                    edge->to->position.x + offset2.x, edge->to->position.y, edge->to->position.z + offset2.z
                };
                
                glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices2), lineVertices2, GL_DYNAMIC_DRAW);
                glDrawArrays(GL_LINES, 0, 2);
            } else {
                // One-way: Single red line
                m_Shader->SetFloat4("u_Color", glm::vec4(0.5f, 0.3f, 0.3f, 1.0f));
                
                float lineVertices[] = {
                    node->position.x, node->position.y, node->position.z,
                    edge->to->position.x, edge->to->position.y, edge->to->position.z
                };
                
                glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
                
                glm::mat4 model = glm::mat4(1.0f);
                m_Shader->SetMat4("u_Model", model);
                glDrawArrays(GL_LINES, 0, 2);
            }
        }
    }
}

void Application::RenderVehicles() {
    const auto& vehicles = m_Simulation->GetVehicles();
    
    for (size_t i = 0; i < vehicles.size(); i++) {
        const auto& vehicle = vehicles[i];
        
        float hue = (i * 60.0f) / 360.0f;
        glm::vec3 color(
            0.5f + 0.5f * sin(hue * 6.28f),
            0.5f + 0.5f * sin((hue + 0.33f) * 6.28f),
            0.5f + 0.5f * sin((hue + 0.67f) * 6.28f)
        );
        
        m_Shader->SetFloat4("u_Color", glm::vec4(color, 1.0f));
        
        float triangleVertices[] = {
             0.0f,  0.5f,  1.0f,
            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f
        };
        
        glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
        glBindVertexArray(m_LineVAO);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, vehicle->GetPosition());
        
        glm::vec3 velocity = vehicle->GetVelocity();
        if (glm::length(velocity) > 0.01f) {
            glm::vec3 direction = glm::normalize(velocity);
            float angle = atan2(direction.x, direction.z);
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        
        m_Shader->SetMat4("u_Model", model);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

void Application::RenderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 450), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Transport Simulator - Stats", nullptr, ImGuiWindowFlags_NoCollapse);
    
    ImGui::Text("3D Transport Simulator");
    ImGui::Separator();
    
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "Controls");
    ImGui::Text("SPACE: Toggle Auto/Manual Camera");
    ImGui::Text("WASD: Move | QE: Up/Down");
    ImGui::Text("Arrow Keys: Rotate Camera");
    ImGui::Separator();
    
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "Network");
    auto graph = m_Simulation->GetGraph();
    ImGui::Text("Nodes: %zu", graph->GetNodeCount());
    ImGui::Text("Grid: 12x12 (Single Level)");
    
    int totalEdges = 0, oneWayEdges = 0, twoWayEdges = 0;
    for (const auto& [id, node] : graph->GetNodes()) {
        for (const auto& edge : node->edges) {
            totalEdges++;
            bool isBi = false;
            for (const auto& re : edge->to->edges) {
                if (re->to->id == node->id) { isBi = true; break; }
            }
            if (isBi) twoWayEdges++; else oneWayEdges++;
        }
    }
    
    ImGui::Text("Total Roads: %d", totalEdges);
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 0.5f, 1.0f), "  Two-Way: %d", twoWayEdges / 2);
    ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.5f, 1.0f), "  One-Way: %d", oneWayEdges);
    ImGui::Separator();
    
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.4f, 1.0f), "Vehicles");
    const auto& vehicles = m_Simulation->GetVehicles();
    ImGui::Text("Active: %zu", vehicles.size());
    
    int moving = 0, stopped = 0;
    for (const auto& v : vehicles) {
        if (v->IsMoving()) moving++; else stopped++;
    }
    
    ImGui::Text("  Moving: %d | Stopped: %d", moving, stopped);
    ImGui::Separator();
    
    ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.8f, 1.0f), "Performance");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    
    ImGui::End();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}