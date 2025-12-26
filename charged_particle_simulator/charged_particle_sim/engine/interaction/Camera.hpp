#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Orbit Camera System
 * 
 * Provides spherical coordinate camera that orbits around a configurable target point.
 * Supports mouse dragging for rotation and scroll wheel for zooming.
 */
class Camera {
public:
    // Constructor with configurable orbit center
    Camera(const glm::vec3& orbitCenter = glm::vec3(0.0f));
    
    // Get current camera position in world space
    glm::vec3 position() const;
    
    // Get view matrix (lookAt matrix)
    glm::mat4 getViewMatrix() const;
    
    // Get target point (orbit center)
    glm::vec3 getTarget() const { return target; }
    
    // Set orbit center (target point)
    void setTarget(const glm::vec3& newTarget);
    
    // Get camera radius (distance from target)
    float getRadius() const { return radius; }
    
    // Set camera radius with clamping
    void setRadius(float newRadius);
    
    // Get azimuth angle (horizontal rotation)
    float getAzimuth() const { return azimuth; }
    
    // Get elevation angle (vertical rotation)
    float getElevation() const { return elevation; }
    
    // Set azimuth and elevation angles
    void setAngles(float newAzimuth, float newElevation);
    
    // Input handling
    void processMouseMove(double x, double y);
    void processMouseButton(int button, int action, int mods, GLFWwindow* window);
    void processScroll(double xoffset, double yoffset);
    void processKey(int key, int scancode, int action, int mods);
    
    // Check if camera is currently moving (for optimization)
    bool isMoving() const { return moving; }
    
    // Update camera state (call each frame)
    void update();

private:
    // Orbit center (target point)
    glm::vec3 target;
    
    // Spherical coordinates
    float radius;           // Distance from target
    float minRadius;        // Minimum zoom distance
    float maxRadius;        // Maximum zoom distance
    float azimuth;          // Horizontal angle (radians)
    float elevation;        // Vertical angle (radians, 0 = top, π = bottom)
    
    // Input sensitivity
    float orbitSpeed;       // Mouse drag sensitivity for rotation
    float panSpeed;         // Pan sensitivity (currently unused)
    float zoomSpeed;        // Scroll wheel zoom sensitivity
    
    // Input state
    bool dragging;          // Left mouse button is held
    bool panning;           // Middle mouse button is held (for future pan support)
    bool moving;            // Camera is currently being moved (optimization flag)
    double lastX;           // Last mouse X position
    double lastY;           // Last mouse Y position
};

