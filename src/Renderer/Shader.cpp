#include "Shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    
    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, vertexShader);
    glAttachShader(m_ProgramID, fragmentShader);
    glLinkProgram(m_ProgramID);
    
    int success;
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_ProgramID, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed:\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(m_ProgramID);
}

void Shader::Bind() const {
    glUseProgram(m_ProgramID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

int Shader::GetUniformLocation(const std::string& name) {
    return glGetUniformLocation(m_ProgramID, name.c_str());
}

void Shader::SetInt(const std::string& name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetFloat3(const std::string& name, const glm::vec3& value) {
    glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

void Shader::SetFloat4(const std::string& name, const glm::vec4& value) {
    glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
