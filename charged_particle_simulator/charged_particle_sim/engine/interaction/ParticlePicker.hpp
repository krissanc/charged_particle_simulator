#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "RayCaster.hpp"
#include "engine/physics/Particle.hpp"

/**
 * Pick Result
 * 
 * Contains information about a ray-particle intersection.
 */
struct PickResult {
    bool hit;                   // True if ray hit a particle
    int particleIndex;           // Index of hit particle (-1 if no hit)
    glm::dvec3 hitPoint;         // World space hit point
    float distance;              // Distance from ray origin to hit point
    
    PickResult() : hit(false), particleIndex(-1), hitPoint(0.0), distance(std::numeric_limits<float>::max()) {}
};

/**
 * Particle Picker
 * 
 * Performs ray-sphere intersection tests to pick particles with mouse.
 */
class ParticlePicker {
public:
    /**
     * Pick nearest particle intersected by ray
     * 
     * @param ray Ray in world space
     * @param particles All particles in scene
     * @return Pick result with hit information
     */
    static PickResult pick(
        const Ray& ray,
        const std::vector<Particle>& particles
    );
    
    /**
     * Check if ray intersects a sphere
     * 
     * @param ray Ray in world space
     * @param center Sphere center
     * @param radius Sphere radius
     * @param hitPoint Output: intersection point (if hit)
     * @return True if ray intersects sphere
     */
    static bool raySphereIntersect(
        const Ray& ray,
        const glm::dvec3& center,
        double radius,
        glm::dvec3& hitPoint
    );
};

