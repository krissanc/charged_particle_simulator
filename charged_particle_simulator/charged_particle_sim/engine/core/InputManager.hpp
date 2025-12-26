#pragma once

#include <GLFW/glfw3.h>
#include <functional>

// Forward declarations
class Camera;
class DragController;
class ParticlePicker;
class RayCaster;
struct Particle;  // Particle is a struct

/**
 * Input Manager
 * 
 * Wires GLFW input callbacks to camera, drag controller, and other systems.
 * Centralizes input handling for the application.
 */
class InputManager {
public:
    InputManager();
    
    /**
     * Setup all input callbacks for a window
     * 
     * @param window GLFW window
     * @param camera Camera to control
     * @param dragController Drag controller for particle interaction
     * @param particlePicker Picker for ray casting
     * @param particles Particle list (for picking)
     */
    void setupCallbacks(
        GLFWwindow* window,
        Camera* camera,
        DragController* dragController,
        ParticlePicker* particlePicker,
        std::vector<Particle>* particles
    );
    
    /**
     * Update input (call each frame)
     * Handles continuous input like mouse movement
     */
    void update(GLFWwindow* window);

private:
    // Callback state
    Camera* m_camera;
    DragController* m_dragController;
    ParticlePicker* m_picker;
    std::vector<Particle>* m_particles;
    
    // Mouse state
    bool m_leftMousePressed;
    double m_lastMouseX;
    double m_lastMouseY;
    
    // Window dimensions (for ray casting)
    int m_windowWidth;
    int m_windowHeight;
    
    // Static callback wrappers (GLFW requires C-style callbacks)
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* win, double x, double y);
    static void scrollCallback(GLFWwindow* win, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    
    // Get InputManager instance from window user pointer
    static InputManager* getInstance(GLFWwindow* window);
};

