#include "RayCaster.hpp"
#include <glm/gtc/matrix_inverse.hpp>

Ray RayCaster::screenToWorldRay(
    double mouseX,
    double mouseY,
    int screenWidth,
    int screenHeight,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix
) {
    // Normalize mouse coordinates to [-1, 1]
    float x = (2.0f * static_cast<float>(mouseX)) / static_cast<float>(screenWidth) - 1.0f;
    float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / static_cast<float>(screenHeight);
    
    // Create ray in clip space
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);
    
    // Transform to eye space
    glm::mat4 invProjection = glm::inverse(projectionMatrix);
    glm::vec4 rayEye = invProjection * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    
    // Transform to world space
    glm::mat4 invView = glm::inverse(viewMatrix);
    glm::vec4 rayWorld = invView * rayEye;
    
    // Extract origin and direction
    glm::vec3 origin = glm::vec3(invView[3]);  // Camera position from view matrix
    glm::vec3 direction = glm::normalize(glm::vec3(rayWorld));
    
    return Ray(origin, direction);
}

Ray RayCaster::fromCamera(
    const glm::vec3& cameraPos,
    const glm::vec3& direction
) {
    return Ray(cameraPos, direction);
}

