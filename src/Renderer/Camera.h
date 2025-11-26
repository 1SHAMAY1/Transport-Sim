#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float fov, float aspectRatio, float nearClip, float farClip);
    ~Camera() = default;
    
    void Update(float deltaTime);
    
    const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    const glm::vec3& GetPosition() const { return m_Position; }
    
    void SetPosition(const glm::vec3& position);
    void SetRotation(float pitch, float yaw);
    void SetAspectRatio(float aspectRatio);
    
    // Camera movement
    void MoveForward(float amount);
    void MoveRight(float amount);
    void MoveUp(float amount);
    
private:
    void RecalculateViewMatrix();
    
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;
    
    glm::vec3 m_Position = glm::vec3(0.0f, 10.0f, 20.0f);
    glm::vec3 m_Forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
    
    float m_Pitch = 0.0f;
    float m_Yaw = -90.0f;
    
    float m_Fov;
    float m_AspectRatio;
    float m_NearClip;
    float m_FarClip;
};
