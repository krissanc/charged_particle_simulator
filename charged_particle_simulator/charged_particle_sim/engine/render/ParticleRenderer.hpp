#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "engine/physics/Particle.hpp"

// Forward declarations
struct GLFWwindow;
typedef unsigned int GLuint;

/**
 * Particle Renderer
 * 
 * Renders charged particles as spheres using instanced rendering.
 * Generates UV sphere mesh and renders multiple particles efficiently.
 */
class ParticleRenderer {
public:
    ParticleRenderer();
    ~ParticleRenderer();
    
    // Initialize renderer (call after OpenGL context is created)
    bool initialize();
    
    // Cleanup OpenGL resources
    void cleanup();
    
    // Render all particles
    void render(
        const std::vector<Particle>& particles,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix
    );
    
    // Set visual scale (for scaling particle sizes)
    void setVisualScale(float scale) { m_visualScale = scale; }
    
    // Get visual scale
    float getVisualScale() const { return m_visualScale; }

private:
    // Generate UV sphere mesh
    void generateSphereMesh(int segments, int rings);
    
    // Load and compile shaders
    bool loadShaders();
    
    // OpenGL resources
    GLuint m_VAO;              // Vertex Array Object
    GLuint m_VBO;              // Vertex Buffer Object (sphere vertices)
    GLuint m_EBO;              // Element Buffer Object (indices)
    GLuint m_instanceVBO;      // Instance Buffer Object (particle data)
    GLuint m_shaderProgram;    // Shader program
    
    // Mesh data
    std::vector<float> m_vertices;      // Sphere vertex positions
    std::vector<unsigned int> m_indices; // Sphere indices
    int m_indexCount;                    // Number of indices
    
    // Rendering state
    bool m_initialized;
    float m_visualScale;        // Scale factor for particle sizes
    
    // Shader uniform locations
    GLuint m_viewLoc;
    GLuint m_projectionLoc;
    GLuint m_visualScaleLoc;
};

