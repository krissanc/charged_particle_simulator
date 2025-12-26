#include "FieldLineRenderer.hpp"
#include "engine/core/Logger.hpp"
#include <glad/glad.h>
#include <algorithm>
#include <cmath>

FieldLineRenderer::FieldLineRenderer()
    : m_VAO(0)
    , m_VBO(0)
    , m_colorVBO(0)
    , m_progressVBO(0)
    , m_shaderProgram(0)
    , m_initialized(false)
    , m_maxFieldMagnitude(1.0f)
    , m_viewLoc(0)
    , m_projectionLoc(0)
    , m_timeLoc(0)
    , m_maxFieldMagLoc(0)
{
}

FieldLineRenderer::~FieldLineRenderer() {
    cleanup();
}

bool FieldLineRenderer::initialize() {
    if (m_initialized) {
        LOG_WARN("FieldLineRenderer already initialized");
        return true;
    }
    
    LOG_INFO("Initializing FieldLineRenderer...");
    
    // Load shaders
    if (!loadShaders()) {
        LOG_ERROR("Failed to load shaders for FieldLineRenderer");
        return false;
    }
    
    // Create VAO and VBOs
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_colorVBO);
    glGenBuffers(1, &m_progressVBO);
    
    // Bind VAO
    glBindVertexArray(m_VAO);
    
    // Position buffer (will be updated each frame)
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    // Color buffer (field magnitude)
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    
    // Progress buffer (0.0 to 1.0 along line)
    glBindBuffer(GL_ARRAY_BUFFER, m_progressVBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    
    glBindVertexArray(0);
    
    // Get uniform locations
    m_viewLoc = glGetUniformLocation(m_shaderProgram, "view");
    m_projectionLoc = glGetUniformLocation(m_shaderProgram, "projection");
    m_timeLoc = glGetUniformLocation(m_shaderProgram, "time");
    m_maxFieldMagLoc = glGetUniformLocation(m_shaderProgram, "maxFieldMagnitude");
    
    m_initialized = true;
    LOG_INFO("FieldLineRenderer initialized successfully");
    
    return true;
}

void FieldLineRenderer::cleanup() {
    if (!m_initialized) return;
    
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_colorVBO) glDeleteBuffers(1, &m_colorVBO);
    if (m_progressVBO) glDeleteBuffers(1, &m_progressVBO);
    if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
    
    m_VAO = m_VBO = m_colorVBO = m_progressVBO = m_shaderProgram = 0;
    m_initialized = false;
    
    LOG_INFO("FieldLineRenderer cleaned up");
}

bool FieldLineRenderer::loadShaders() {
    // Vertex shader source
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in float aFieldMagnitude;
        layout (location = 2) in float aLineProgress;
        
        uniform mat4 view;
        uniform mat4 projection;
        uniform float time;
        uniform float maxFieldMagnitude;
        
        out float vFieldMagnitude;
        out float vLineProgress;
        out float vTime;
        
        void main() {
            gl_Position = projection * view * vec4(aPosition, 1.0);
            vFieldMagnitude = aFieldMagnitude / max(maxFieldMagnitude, 0.001);
            vLineProgress = aLineProgress;
            vTime = time;
        }
    )";
    
    // Fragment shader source
    const char* fragmentShaderSource = R"(
        #version 330 core
        in float vFieldMagnitude;
        in float vLineProgress;
        in float vTime;
        
        out vec4 FragColor;
        
        void main() {
            // Color gradient based on field strength
            // High field = bright, low field = dim
            float intensity = clamp(vFieldMagnitude, 0.1, 1.0);
            
            // Direction encoding: animated stripe pattern
            float stripe = mod(vLineProgress * 50.0 + vTime * 2.0, 1.0);
            float arrow = smoothstep(0.4, 0.5, stripe) - smoothstep(0.5, 0.6, stripe);
            
            // Base color: white/cyan for field lines
            vec3 baseColor = vec3(0.7, 0.9, 1.0);
            
            // Apply intensity
            vec3 color = baseColor * intensity;
            
            // Add directional arrows (brighter stripes)
            color += vec3(0.3) * arrow;
            
            FragColor = vec4(color, 0.8);
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
        LOG_ERROR("Field line vertex shader compilation failed: " + std::string(infoLog));
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
        LOG_ERROR("Field line fragment shader compilation failed: " + std::string(infoLog));
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
        LOG_ERROR("Field line shader program linking failed: " + std::string(infoLog));
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
        return false;
    }
    
    // Cleanup shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

void FieldLineRenderer::updateBuffers(const std::vector<FieldLine>& fieldLines) {
    m_positions.clear();
    m_colors.clear();
    m_progress.clear();
    m_lineLengths.clear();
    
    // Flatten all field lines into single buffers
    for (const auto& line : fieldLines) {
        if (line.points.empty()) continue;
        
        int pointCount = static_cast<int>(line.points.size());
        m_lineLengths.push_back(pointCount);
        
        for (int i = 0; i < pointCount; ++i) {
            // Position
            m_positions.push_back(static_cast<float>(line.points[i].x));
            m_positions.push_back(static_cast<float>(line.points[i].y));
            m_positions.push_back(static_cast<float>(line.points[i].z));
            
            // Field magnitude
            m_colors.push_back(line.fieldMagnitudes[i]);
            
            // Progress along line (0.0 to 1.0)
            m_progress.push_back(static_cast<float>(i) / static_cast<float>(pointCount - 1));
        }
    }
    
    // Update max field magnitude if needed
    if (!m_colors.empty()) {
        float currentMax = *std::max_element(m_colors.begin(), m_colors.end());
        if (currentMax > m_maxFieldMagnitude) {
            m_maxFieldMagnitude = currentMax;
        }
    }
}

void FieldLineRenderer::render(
    const std::vector<FieldLine>& fieldLines,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    double time
) {
    if (!m_initialized || fieldLines.empty()) {
        return;
    }
    
    // Update buffers
    updateBuffers(fieldLines);
    
    if (m_positions.empty()) {
        return;
    }
    
    // Upload position data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 m_positions.size() * sizeof(float), 
                 m_positions.data(), 
                 GL_DYNAMIC_DRAW);
    
    // Upload color data
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 m_colors.size() * sizeof(float), 
                 m_colors.data(), 
                 GL_DYNAMIC_DRAW);
    
    // Upload progress data
    glBindBuffer(GL_ARRAY_BUFFER, m_progressVBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 m_progress.size() * sizeof(float), 
                 m_progress.data(), 
                 GL_DYNAMIC_DRAW);
    
    // Use shader program
    glUseProgram(m_shaderProgram);
    
    // Set uniforms
    glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(m_projectionLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
    glUniform1f(m_timeLoc, static_cast<float>(time));
    glUniform1f(m_maxFieldMagLoc, m_maxFieldMagnitude);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Disable depth writing for field lines (they should be visible through particles)
    glDepthMask(GL_FALSE);
    
    // Bind VAO and draw
    glBindVertexArray(m_VAO);
    
    // Draw each line separately (GL_LINE_STRIP)
    int offset = 0;
    for (int length : m_lineLengths) {
        glDrawArrays(GL_LINE_STRIP, offset, length);
        offset += length;
    }
    
    glBindVertexArray(0);
    
    // Restore depth writing
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

