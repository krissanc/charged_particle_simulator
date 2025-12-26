#pragma once

#include <vector>
#include "engine/physics/Particle.hpp"
#include "engine/physics/ElectricField.hpp"
#include "engine/math/Integrators.hpp"

/**
 * Particle System
 * 
 * Manages collection of particles and their physics simulation.
 * Handles force calculation, integration, and collision prevention.
 */
class ParticleSystem {
public:
    ParticleSystem();
    
    /**
     * Add a particle to the system
     */
    void addParticle(const Particle& particle);
    
    /**
     * Remove a particle by index
     */
    void removeParticle(int index);
    
    /**
     * Get all particles (const reference)
     */
    const std::vector<Particle>& getParticles() const { return m_particles; }
    
    /**
     * Get all particles (non-const reference for modification)
     */
    std::vector<Particle>& getParticles() { return m_particles; }
    
    /**
     * Get particle count
     */
    size_t getParticleCount() const { return m_particles.size(); }
    
    /**
     * Step physics simulation forward by dt seconds
     * 
     * @param dt Time step in seconds
     */
    void step(double dt);
    
    /**
     * Reset all particles to initial state
     */
    void reset();
    
    /**
     * Set integration method
     */
    enum class IntegrationMethod {
        EULER,
        VERLET
    };
    void setIntegrationMethod(IntegrationMethod method) { m_integrationMethod = method; }
    
    /**
     * Enable/disable collision prevention
     */
    void setCollisionPrevention(bool enabled) { m_collisionPrevention = enabled; }
    
    /**
     * Set minimum separation distance for collision prevention
     */
    void setMinSeparation(double minSep) { m_minSeparation = minSep; }

private:
    std::vector<Particle> m_particles;
    std::vector<Particle> m_initialParticles;  // For reset
    
    IntegrationMethod m_integrationMethod;
    bool m_collisionPrevention;
    double m_minSeparation;
    
    /**
     * Compute net force on a particle from all other particles
     */
    glm::dvec3 computeNetForce(const Particle& target) const;
    
    /**
     * Apply collision prevention (soft repulsion)
     */
    void applyCollisionPrevention();
    
    /**
     * Clamp velocity to maximum allowed
     */
    void clampVelocities();
};

