#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "engine/physics/FieldLineGenerator.hpp"

// Forward declarations
typedef unsigned int GLuint;

/**
 * Field Line Renderer
 * 
 * Renders electric field lines as polylines with color encoding based on field strength.
 * Supports animated stripe patterns to show field direction.
 */
class FieldLineRenderer {
public:
    FieldLineRenderer();
    ~FieldLineRenderer();
    
    // Initialize renderer (call after OpenGL context is created)
    bool initialize();
    
    // Cleanup OpenGL resources
    void cleanup();
    
    // Render all field lines
    void render(
        const std::vector<FieldLine>& fieldLines,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix,
        double time = 0.0  // Current time for animation
    );
    
    // Set maximum field magnitude for color normalization
    void setMaxFieldMagnitude(float maxMag) { m_maxFieldMagnitude = maxMag; }
    
    // Get maximum field magnitude
    float getMaxFieldMagnitude() const { return m_maxFieldMagnitude; }

private:
    // Load and compile shaders
    bool loadShaders();
    
    // Update field line buffers
    void updateBuffers(const std::vector<FieldLine>& fieldLines);
    
    // OpenGL resources
    GLuint m_VAO;              // Vertex Array Object
    GLuint m_VBO;              // Vertex Buffer Object (positions)
    GLuint m_colorVBO;         // Color Buffer Object (field magnitudes)
    GLuint m_progressVBO;      // Progress Buffer Object (0.0 to 1.0 along line)
    GLuint m_shaderProgram;    // Shader program
    
    // Buffer data
    std::vector<float> m_positions;      // All line points
    std::vector<float> m_colors;          // Field magnitudes
    std::vector<float> m_progress;       // Progress along line (0.0 to 1.0)
    std::vector<int> m_lineLengths;      // Number of points per line
    
    // Rendering state
    bool m_initialized;
    float m_maxFieldMagnitude;  // Maximum field magnitude for normalization
    
    // Shader uniform locations
    GLuint m_viewLoc;
    GLuint m_projectionLoc;
    GLuint m_timeLoc;
    GLuint m_maxFieldMagLoc;
};

