#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <chrono>
#include "engine/physics/Particle.hpp"

/**
 * Drag Controller
 * 
 * Manages particle dragging interaction.
 * Implements state machine: IDLE -> DRAGGING -> RELEASING
 * Infers velocity from motion history on release.
 */
class DragController {
public:
    enum class State {
        IDLE,       // Waiting for click
        DRAGGING,   // Particle is being dragged
        RELEASING   // Just released, inferring velocity
    };
    
    DragController();
    
    /**
     * Begin dragging a particle
     * 
     * @param particle Particle to drag (non-const reference)
     * @param hitPoint Initial hit point from ray intersection
     * @param cameraPos Camera position for drag plane calculation
     */
    void beginDrag(Particle& particle, const glm::dvec3& hitPoint, const glm::dvec3& cameraPos);
    
    /**
     * Update drag position
     * 
     * @param ray Current mouse ray
     */
    void updateDrag(const Ray& ray);
    
    /**
     * End dragging and infer release velocity
     */
    void endDrag();
    
    /**
     * Get current state
     */
    State getState() const { return m_state; }
    
    /**
     * Check if currently dragging
     */
    bool isDragging() const { return m_state == State::DRAGGING; }
    
    /**
     * Get currently dragged particle (nullptr if not dragging)
     */
    Particle* getDraggedParticle() const { return m_particle; }
    
    /**
     * Set maximum release velocity (m/s)
     */
    void setMaxReleaseSpeed(double maxSpeed) { m_maxReleaseSpeed = maxSpeed; }

private:
    // State
    State m_state;
    Particle* m_particle;  // Currently dragged particle (null if none)
    
    // Drag plane (for projecting mouse movement)
    glm::dvec3 m_dragPlaneNormal;
    glm::dvec3 m_dragPlanePoint;
    glm::dvec3 m_cameraPosition;
    
    // Motion history (for velocity inference)
    struct MotionPoint {
        glm::dvec3 position;
        double timestamp;
    };
    std::vector<MotionPoint> m_positionHistory;
    static constexpr size_t MAX_HISTORY = 10;
    
    // Velocity cap
    double m_maxReleaseSpeed;
    
    /**
     * Get current time in seconds
     */
    double getCurrentTime() const;
    
    /**
     * Calculate velocity from motion history
     */
    glm::dvec3 inferVelocity() const;
    
    /**
     * Ray-plane intersection
     */
    bool rayPlaneIntersect(
        const Ray& ray,
        const glm::dvec3& planePoint,
        const glm::dvec3& planeNormal,
        glm::dvec3& intersection
    ) const;
};

// Forward declaration
struct Ray;

