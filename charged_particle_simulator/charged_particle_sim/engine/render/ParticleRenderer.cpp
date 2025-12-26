#include "ParticleRenderer.hpp"
#include "engine/core/Logger.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ParticleRenderer::ParticleRenderer()
    : m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_instanceVBO(0)
    , m_shaderProgram(0)
    , m_indexCount(0)
    , m_initialized(false)
    , m_visualScale(1.0f)
    , m_viewLoc(0)
    , m_projectionLoc(0)
    , m_visualScaleLoc(0)
{
}

ParticleRenderer::~ParticleRenderer() {
    cleanup();
}

bool ParticleRenderer::initialize() {
    if (m_initialized) {
        LOG_WARN("ParticleRenderer already initialized");
        return true;
    }
    
    LOG_INFO("Initializing ParticleRenderer...");
    
    // Generate sphere mesh
    generateSphereMesh(16, 16); // 16 segments, 16 rings
    
    // Load shaders
    if (!loadShaders()) {
        LOG_ERROR("Failed to load shaders for ParticleRenderer");
        return false;
    }
    
    // Create VAO, VBO, EBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glGenBuffers(1, &m_instanceVBO);
    
    // Bind VAO
    glBindVertexArray(m_VAO);
    
    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 m_vertices.size() * sizeof(float), 
                 m_vertices.data(), 
                 GL_STATIC_DRAW);
    
    // Vertex position attribute (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 m_indices.size() * sizeof(unsigned int), 
                 m_indices.data(), 
                 GL_STATIC_DRAW);
    
    // Instance data will be uploaded each frame
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    
    // Instance position (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1); // Update once per instance
    
    // Instance radius (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribDivisor(2, 1);
    
    // Instance color (location 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
    glVertexAttribDivisor(3, 1);
    
    glBindVertexArray(0);
    
    // Get uniform locations
    m_viewLoc = glGetUniformLocation(m_shaderProgram, "view");
    m_projectionLoc = glGetUniformLocation(m_shaderProgram, "projection");
    m_visualScaleLoc = glGetUniformLocation(m_shaderProgram, "visualScale");
    
    m_initialized = true;
    LOG_INFO("ParticleRenderer initialized successfully");
    
    return true;
}

void ParticleRenderer::cleanup() {
    if (!m_initialized) return;
    
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
    if (m_instanceVBO) glDeleteBuffers(1, &m_instanceVBO);
    if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
    
    m_VAO = m_VBO = m_EBO = m_instanceVBO = m_shaderProgram = 0;
    m_initialized = false;
    
    LOG_INFO("ParticleRenderer cleaned up");
}

void ParticleRenderer::generateSphereMesh(int segments, int rings) {
    m_vertices.clear();
    m_indices.clear();
    
    // Generate vertices
    for (int ring = 0; ring <= rings; ++ring) {
        float theta = static_cast<float>(ring) * static_cast<float>(M_PI) / static_cast<float>(rings);
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        
        for (int seg = 0; seg <= segments; ++seg) {
            float phi = static_cast<float>(seg) * 2.0f * static_cast<float>(M_PI) / static_cast<float>(segments);
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            
            // Sphere vertex position (unit sphere)
            float x = sinTheta * cosPhi;
            float y = cosTheta;
            float z = sinTheta * sinPhi;
            
            m_vertices.push_back(x);
            m_vertices.push_back(y);
            m_vertices.push_back(z);
        }
    }
    
    // Generate indices
    for (int ring = 0; ring < rings; ++ring) {
        for (int seg = 0; seg < segments; ++seg) {
            int first = ring * (segments + 1) + seg;
            int second = first + segments + 1;
            
            // First triangle
            m_indices.push_back(first);
            m_indices.push_back(second);
            m_indices.push_back(first + 1);
            
            // Second triangle
            m_indices.push_back(first + 1);
            m_indices.push_back(second);
            m_indices.push_back(second + 1);
        }
    }
    
    m_indexCount = static_cast<int>(m_indices.size());
}

bool ParticleRenderer::loadShaders() {
    // Vertex shader source
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aInstancePos;
        layout (location = 2) in float aInstanceRadius;
        layout (location = 3) in vec3 aInstanceColor;
        
        uniform mat4 view;
        uniform mat4 projection;
        uniform float visualScale;
        
        out vec3 FragColor;
        
        void main() {
            // Scale vertex by instance radius and visual scale
            vec3 worldPos = aInstancePos + aPos * aInstanceRadius * visualScale;
            gl_Position = projection * view * vec4(worldPos, 1.0);
            FragColor = aInstanceColor;
        }
    )";
    
    // Fragment shader source
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 FragColor;
        out vec4 color;
        
        void main() {
            color = vec4(FragColor, 1.0);
        }
    )";
    
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    // Check compilation
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        LOG_ERROR("Vertex shader compilation failed: " + std::string(infoLog));
        glDeleteShader(vertexShader);
        return false;
    }
    
    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    
    // Check compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        LOG_ERROR("Fragment shader compilation failed: " + std::string(infoLog));
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    
    // Link shader program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);
    
    // Check linking
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
        LOG_ERROR("Shader program linking failed: " + std::string(infoLog));
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
        return false;
    }
    
    // Cleanup shaders (no longer needed after linking)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

void ParticleRenderer::render(
    const std::vector<Particle>& particles,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix
) {
    if (!m_initialized || particles.empty()) {
        return;
    }
    
    // Prepare instance data: [pos.x, pos.y, pos.z, radius, color.r, color.g, color.b]
    std::vector<float> instanceData;
    instanceData.reserve(particles.size() * 7);
    
    for (const auto& particle : particles) {
        // Position
        instanceData.push_back(static_cast<float>(particle.position.x));
        instanceData.push_back(static_cast<float>(particle.position.y));
        instanceData.push_back(static_cast<float>(particle.position.z));
        
        // Radius
        instanceData.push_back(particle.visualRadius);
        
        // Color
        instanceData.push_back(particle.color.r);
        instanceData.push_back(particle.color.g);
        instanceData.push_back(particle.color.b);
    }
    
    // Upload instance data
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 instanceData.size() * sizeof(float), 
                 instanceData.data(), 
                 GL_DYNAMIC_DRAW);
    
    // Use shader program
    glUseProgram(m_shaderProgram);
    
    // Set uniforms
    glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(m_projectionLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
    glUniform1f(m_visualScaleLoc, m_visualScale);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Bind VAO and draw
    glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(particles.size()));
    glBindVertexArray(0);
}

