#include "Camera.hpp"
#include "engine/core/Logger.hpp"

Camera::Camera(const glm::vec3& orbitCenter)
    : target(orbitCenter)
    , radius(10.0f)
    , minRadius(0.1f)
    , maxRadius(1000.0f)
    , azimuth(0.0f)
    , elevation(static_cast<float>(M_PI) / 2.0f)
    , orbitSpeed(0.01f)
    , panSpeed(0.01f)
    , zoomSpeed(1.0f)
    , dragging(false)
    , panning(false)
    , moving(false)
    , lastX(0.0)
    , lastY(0.0)
{
    LOG_DEBUG("Camera initialized with orbit center at (" + 
              std::to_string(orbitCenter.x) + ", " +
              std::to_string(orbitCenter.y) + ", " +
              std::to_string(orbitCenter.z) + ")");
}

glm::vec3 Camera::position() const {
    // Clamp elevation to avoid gimbal lock at poles
    float clampedElevation = glm::clamp(elevation, 0.01f, static_cast<float>(M_PI) - 0.01f);
    
    // Convert spherical coordinates to Cartesian
    // X = radius * sin(elevation) * cos(azimuth)
    // Y = radius * cos(elevation)
    // Z = radius * sin(elevation) * sin(azimuth)
    return target + glm::vec3(
        radius * sin(clampedElevation) * cos(azimuth),
        radius * cos(clampedElevation),
        radius * sin(clampedElevation) * sin(azimuth)
    );
}

glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 pos = position();
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // Y-axis is up
    return glm::lookAt(pos, target, up);
}

void Camera::setTarget(const glm::vec3& newTarget) {
    target = newTarget;
    update();
}

void Camera::setRadius(float newRadius) {
    radius = glm::clamp(newRadius, minRadius, maxRadius);
    update();
}

void Camera::setAngles(float newAzimuth, float newElevation) {
    azimuth = newAzimuth;
    elevation = glm::clamp(newElevation, 0.01f, static_cast<float>(M_PI) - 0.01f);
    update();
}

void Camera::processMouseMove(double x, double y) {
    float dx = static_cast<float>(x - lastX);
    float dy = static_cast<float>(y - lastY);
    
    if (dragging && !panning) {
        // Orbit: Left mouse drag rotates camera
        azimuth += dx * orbitSpeed;
        elevation -= dy * orbitSpeed; // Invert Y for intuitive control
        elevation = glm::clamp(elevation, 0.01f, static_cast<float>(M_PI) - 0.01f);
    }
    // Future: panning support could be added here
    
    lastX = x;
    lastY = y;
    update();
}

void Camera::processMouseButton(int button, int action, int mods, GLFWwindow* window) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            dragging = true;
            panning = false;
            glfwGetCursorPos(window, &lastX, &lastY);
            LOG_DEBUG("Camera drag started");
        } else if (action == GLFW_RELEASE) {
            dragging = false;
            LOG_DEBUG("Camera drag ended");
        }
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        // Middle mouse button reserved for future panning
        if (action == GLFW_PRESS) {
            panning = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        } else if (action == GLFW_RELEASE) {
            panning = false;
        }
    }
    
    update();
}

void Camera::processScroll(double xoffset, double yoffset) {
    // Zoom in/out by adjusting radius
    radius -= static_cast<float>(yoffset) * zoomSpeed;
    radius = glm::clamp(radius, minRadius, maxRadius);
    update();
    
    LOG_DEBUG("Camera zoom: radius = " + std::to_string(radius));
}

void Camera::processKey(int key, int scancode, int action, int mods) {
    // Keyboard controls can be added here
    // For example: R to reset camera, arrow keys to rotate, etc.
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_R:
                // Reset camera to default position
                azimuth = 0.0f;
                elevation = static_cast<float>(M_PI) / 2.0f;
                radius = 10.0f;
                update();
                LOG_INFO("Camera reset to default position");
                break;
            default:
                break;
        }
    }
}

void Camera::update() {
    // Update moving flag for optimization purposes
    // (e.g., reduce quality during camera movement)
    moving = dragging || panning;
}

