#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();
    
    void Bind() const;
    void Unbind() const;
    
    // Uniform setters
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetFloat3(const std::string& name, const glm::vec3& value);
    void SetFloat4(const std::string& name, const glm::vec4& value);
    void SetMat4(const std::string& name, const glm::mat4& value);
    
private:
    unsigned int m_ProgramID;
    int GetUniformLocation(const std::string& name);
};
