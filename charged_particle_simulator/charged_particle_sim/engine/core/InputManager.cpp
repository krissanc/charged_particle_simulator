#include "InputManager.hpp"
#include "engine/interaction/Camera.hpp"
#include "engine/interaction/DragController.hpp"
#include "engine/interaction/ParticlePicker.hpp"
#include "engine/interaction/RayCaster.hpp"
#include "engine/physics/Particle.hpp"
#include "engine/core/Logger.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

InputManager::InputManager()
    : m_camera(nullptr)
    , m_dragController(nullptr)
    , m_picker(nullptr)
    , m_particles(nullptr)
    , m_leftMousePressed(false)
    , m_lastMouseX(0.0)
    , m_lastMouseY(0.0)
    , m_windowWidth(1280)
    , m_windowHeight(720)
{
}

void InputManager::setupCallbacks(
    GLFWwindow* window,
    Camera* camera,
    DragController* dragController,
    ParticlePicker* particlePicker,
    std::vector<Particle>* particles
) {
    m_camera = camera;
    m_dragController = dragController;
    m_picker = particlePicker;
    m_particles = particles;
    
    // Store InputManager instance in window user pointer
    glfwSetWindowUserPointer(window, this);
    
    // Setup GLFW callbacks
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // Get initial window size
    glfwGetFramebufferSize(window, &m_windowWidth, &m_windowHeight);
    
    LOG_INFO("InputManager callbacks setup complete");
}

void InputManager::update(GLFWwindow* window) {
    // Handle continuous mouse movement for dragging
    if (m_leftMousePressed && m_dragController && m_dragController->isDragging()) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        
        // Get view and projection matrices for ray casting
        if (m_camera && m_picker && m_particles) {
            glm::mat4 view = m_camera->getViewMatrix();
            glm::mat4 projection = glm::perspective(
                glm::radians(60.0f),
                static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight),
                0.1f,
                1000.0f
            );
            
            // Cast ray from mouse position
            Ray ray = RayCaster::screenToWorldRay(x, y, m_windowWidth, m_windowHeight, view, projection);
            
            // Update drag
            m_dragController->updateDrag(ray);
        }
    }
}

InputManager* InputManager::getInstance(GLFWwindow* window) {
    return static_cast<InputManager*>(glfwGetWindowUserPointer(window));
}

void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    InputManager* manager = getInstance(window);
    if (!manager) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            manager->m_leftMousePressed = true;
            glfwGetCursorPos(window, &manager->m_lastMouseX, &manager->m_lastMouseY);
            
            // Try to pick a particle
            if (manager->m_camera && manager->m_picker && manager->m_dragController && manager->m_particles) {
                glm::mat4 view = manager->m_camera->getViewMatrix();
                glm::mat4 projection = glm::perspective(
                    glm::radians(60.0f),
                    static_cast<float>(manager->m_windowWidth) / static_cast<float>(manager->m_windowHeight),
                    0.1f,
                    1000.0f
                );
                
                // Cast ray
                Ray ray = RayCaster::screenToWorldRay(
                    manager->m_lastMouseX,
                    manager->m_lastMouseY,
                    manager->m_windowWidth,
                    manager->m_windowHeight,
                    view,
                    projection
                );
                
                // Pick particle (static method)
                PickResult result = ParticlePicker::pick(ray, *manager->m_particles);
                
                if (result.hit && result.particleIndex >= 0 && 
                    result.particleIndex < static_cast<int>(manager->m_particles->size())) {
                    // Begin dragging
                    Particle& particle = (*manager->m_particles)[result.particleIndex];
                    glm::dvec3 cameraPos = manager->m_camera->position();
                    manager->m_dragController->beginDrag(particle, result.hitPoint, cameraPos);
                    
                    LOG_DEBUG("Picked particle " + std::to_string(result.particleIndex));
                }
            }
            
            // Also forward to camera for orbit control
            if (manager->m_camera) {
                manager->m_camera->processMouseButton(button, action, mods, window);
            }
        } else if (action == GLFW_RELEASE) {
            manager->m_leftMousePressed = false;
            
            // End dragging
            if (manager->m_dragController && manager->m_dragController->isDragging()) {
                manager->m_dragController->endDrag();
            }
            
            // Forward to camera
            if (manager->m_camera) {
                manager->m_camera->processMouseButton(button, action, mods, window);
            }
        }
    } else {
        // Forward other mouse buttons to camera
        if (manager->m_camera) {
            manager->m_camera->processMouseButton(button, action, mods, window);
        }
    }
}

void InputManager::cursorPosCallback(GLFWwindow* win, double x, double y) {
    InputManager* manager = getInstance(win);
    if (!manager) return;
    
    manager->m_lastMouseX = x;
    manager->m_lastMouseY = y;
    
    // Forward to camera for orbit control
    if (manager->m_camera) {
        manager->m_camera->processMouseMove(x, y);
    }
}

void InputManager::scrollCallback(GLFWwindow* win, double xoffset, double yoffset) {
    InputManager* manager = getInstance(win);
    if (!manager) return;
    
    // Forward to camera for zoom
    if (manager->m_camera) {
        manager->m_camera->processScroll(xoffset, yoffset);
    }
}

void InputManager::keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
    InputManager* manager = getInstance(win);
    if (!manager) return;
    
    // Forward to camera
    if (manager->m_camera) {
        manager->m_camera->processKey(key, scancode, action, mods);
    }
}

void InputManager::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    InputManager* manager = getInstance(window);
    if (!manager) return;
    
    manager->m_windowWidth = width;
    manager->m_windowHeight = height;
    glViewport(0, 0, width, height);
}

