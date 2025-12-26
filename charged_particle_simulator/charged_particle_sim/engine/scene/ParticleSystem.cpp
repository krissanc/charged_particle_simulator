#include "ParticleSystem.hpp"
#include "engine/core/Logger.hpp"
#include "engine/core/Constants.hpp"
#include <algorithm>
#include <cmath>

ParticleSystem::ParticleSystem()
    : m_integrationMethod(IntegrationMethod::VERLET)
    , m_collisionPrevention(true)
    , m_minSeparation(1e-12)  // Minimum separation in meters
{
}

void ParticleSystem::addParticle(const Particle& particle) {
    m_particles.push_back(particle);
    m_initialParticles.push_back(particle);
    
    LOG_DEBUG("Added particle: q=" + std::to_string(particle.charge) + 
              " C, m=" + std::to_string(particle.mass) + " kg");
}

void ParticleSystem::removeParticle(int index) {
    if (index >= 0 && index < static_cast<int>(m_particles.size())) {
        m_particles.erase(m_particles.begin() + index);
        if (index < static_cast<int>(m_initialParticles.size())) {
            m_initialParticles.erase(m_initialParticles.begin() + index);
        }
        LOG_DEBUG("Removed particle at index " + std::to_string(index));
    }
}

void ParticleSystem::step(double dt) {
    if (m_particles.empty()) {
        return;
    }
    
    // Compute forces and update accelerations
    for (auto& particle : m_particles) {
        if (particle.isFixed || particle.isBeingDragged) {
            // Skip fixed or dragged particles
            continue;
        }
        
        // Compute net force: F = q * E_total
        glm::dvec3 force = computeNetForce(particle);
        
        // Update acceleration: a = F / m
        particle.acceleration = force / particle.mass;
    }
    
    // Integrate motion
    for (auto& particle : m_particles) {
        if (particle.isFixed || particle.isBeingDragged) {
            continue;
        }
        
        if (m_integrationMethod == IntegrationMethod::VERLET) {
            // Velocity Verlet integration
            VerletResult result = verletStep(
                particle.position,
                particle.velocity,
                particle.acceleration,
                dt
            );
            particle.position = result.position;
            particle.velocity = result.velocity;
        } else {
            // Semi-implicit Euler
            EulerResult result = eulerStep(
                particle.position,
                particle.velocity,
                particle.acceleration,
                dt
            );
            particle.position = result.position;
            particle.velocity = result.velocity;
        }
    }
    
    // Apply collision prevention
    if (m_collisionPrevention) {
        applyCollisionPrevention();
    }
    
    // Clamp velocities to prevent numerical instability
    clampVelocities();
}

void ParticleSystem::reset() {
    m_particles = m_initialParticles;
    
    // Reset velocities and accelerations
    for (auto& particle : m_particles) {
        particle.velocity = glm::dvec3(0.0);
        particle.acceleration = glm::dvec3(0.0);
        particle.isBeingDragged = false;
    }
    
    LOG_INFO("Particle system reset to initial state");
}

glm::dvec3 ParticleSystem::computeNetForce(const Particle& target) const {
    // Compute total electric field at target position
    glm::dvec3 E_total = ElectricField::totalField(target.position, m_particles);
    
    // Force on charged particle: F = q * E
    return target.charge * E_total;
}

void ParticleSystem::applyCollisionPrevention() {
    // Soft repulsion between particles that are too close
    for (size_t i = 0; i < m_particles.size(); ++i) {
        for (size_t j = i + 1; j < m_particles.size(); ++j) {
            Particle& p1 = m_particles[i];
            Particle& p2 = m_particles[j];
            
            // Skip if either is fixed or being dragged
            if (p1.isFixed || p1.isBeingDragged || p2.isFixed || p2.isBeingDragged) {
                continue;
            }
            
            glm::dvec3 r = p2.position - p1.position;
            double distance = glm::length(r);
            
            if (distance < m_minSeparation && distance > 1e-15) {
                // Apply soft repulsion force
                double overlap = m_minSeparation - distance;
                double repulsionStrength = 1e-10;  // Small repulsion constant
                
                glm::dvec3 direction = glm::normalize(r);
                glm::dvec3 repulsionForce = direction * repulsionStrength * overlap;
                
                // Apply force (inverse mass weighting)
                double totalMass = p1.mass + p2.mass;
                p1.acceleration -= repulsionForce / p1.mass;
                p2.acceleration += repulsionForce / p2.mass;
            }
        }
    }
}

void ParticleSystem::clampVelocities() {
    for (auto& particle : m_particles) {
        double speed = glm::length(particle.velocity);
        if (speed > PhysicsConstants::MAX_VELOCITY) {
            particle.velocity = glm::normalize(particle.velocity) * PhysicsConstants::MAX_VELOCITY;
        }
    }
}

