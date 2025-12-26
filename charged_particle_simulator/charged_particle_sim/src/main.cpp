#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

#include "engine/core/Logger.hpp"
#include "engine/core/Constants.hpp"
#include "engine/interaction/Camera.hpp"
#include "engine/physics/Particle.hpp"
#include "engine/physics/ElectricField.hpp"
#include "engine/render/ParticleRenderer.hpp"

// Global state
Camera camera;
ParticleRenderer particleRenderer;
std::vector<Particle> particles;
bool simulationRunning = true;

// Window dimensions
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// GLFW callbacks
void setupCallbacks(GLFWwindow* window) {
    glfwSetWindowUserPointer(window, &camera);
    
    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(win));
        cam->processMouseButton(button, action, mods, win);
    });
    
    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double x, double y) {
        Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(win));
        cam->processMouseMove(x, y);
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
        Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(win));
        cam->processScroll(xoffset, yoffset);
    });
    
    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(win));
        cam->processKey(key, scancode, action, mods);
        
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(win, GL_TRUE);
            } else if (key == GLFW_KEY_SPACE) {
                simulationRunning = !simulationRunning;
                LOG_INFO(simulationRunning ? "Simulation resumed" : "Simulation paused");
            }
        }
    });
}

int main() {
    // Initialize logger
    Logger::initialize("logs/simulation.log", LogLevel::INFO);
    LOG_INFO("=== Charged Particle Simulator Starting ===");
    
    // Initialize GLFW
    if (!glfwInit()) {
        LOG_ERROR("Failed to initialize GLFW");
        return -1;
    }
    
    // Configure OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
                                         "Charged Particle Simulator", nullptr, nullptr);
    if (!window) {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        LOG_ERROR("Failed to initialize GLEW: " + std::string(reinterpret_cast<const char*>(glewGetErrorString(err))));
        glfwTerminate();
        return -1;
    }
    
    LOG_INFO("OpenGL Version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
    
    // Setup callbacks
    setupCallbacks(window);
    
    // Initialize renderer
    if (!particleRenderer.initialize()) {
        LOG_ERROR("Failed to initialize ParticleRenderer");
        glfwTerminate();
        return -1;
    }
    
    // Create test particles
    particles.push_back(Particle::createElectron(glm::dvec3(-1.0, 0.0, 0.0)));
    particles.push_back(Particle::createProton(glm::dvec3(1.0, 0.0, 0.0)));
    
    LOG_INFO("Created " + std::to_string(particles.size()) + " test particles");
    
    // Main loop
    double lastTime = glfwGetTime();
    
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // Clear screen
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Update camera
        camera.update();
        
        // Get view and projection matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(60.0f),
            static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
            0.1f,
            1000.0f
        );
        
        // Render particles
        particleRenderer.render(particles, view, projection);
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    particleRenderer.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    Logger::shutdown();
    LOG_INFO("=== Charged Particle Simulator Shutting Down ===");
    
    return 0;
}

