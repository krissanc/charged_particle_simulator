#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * Ray Structure
 * 
 * Represents a ray in 3D space with origin and direction.
 */
struct Ray {
    glm::vec3 origin;      // Ray origin point
    glm::vec3 direction;   // Ray direction (normalized)
    
    Ray() : origin(0.0f), direction(0.0f, 0.0f, -1.0f) {}
    Ray(const glm::vec3& o, const glm::vec3& d) : origin(o), direction(glm::normalize(d)) {}
};

/**
 * Ray Caster
 * 
 * Converts screen coordinates (mouse position) to world space rays.
 * Used for picking and interaction.
 */
class RayCaster {
public:
    /**
     * Convert screen coordinates to world space ray
     * 
     * @param mouseX Mouse X coordinate in screen space
     * @param mouseY Mouse Y coordinate in screen space
     * @param screenWidth Window width in pixels
     * @param screenHeight Window height in pixels
     * @param viewMatrix Camera view matrix
     * @param projectionMatrix Camera projection matrix
     * @return Ray in world space
     */
    static Ray screenToWorldRay(
        double mouseX,
        double mouseY,
        int screenWidth,
        int screenHeight,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix
    );
    
    /**
     * Get ray origin from camera position
     * 
     * @param cameraPos Camera position in world space
     * @return Ray with origin at camera
     */
    static Ray fromCamera(
        const glm::vec3& cameraPos,
        const glm::vec3& direction
    );
};

