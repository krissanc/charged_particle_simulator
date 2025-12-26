#include "ParticlePicker.hpp"
#include "engine/core/Logger.hpp"
#include <algorithm>
#include <cmath>

PickResult ParticlePicker::pick(
    const Ray& ray,
    const std::vector<Particle>& particles
) {
    PickResult result;
    
    // Test intersection with each particle
    for (int i = 0; i < static_cast<int>(particles.size()); ++i) {
        const auto& particle = particles[i];
        
        glm::dvec3 hitPoint;
        if (raySphereIntersect(ray, particle.position, particle.visualRadius, hitPoint)) {
            // Calculate distance from ray origin to hit point
            float distance = static_cast<float>(glm::length(glm::dvec3(ray.origin) - hitPoint));
            
            // Keep nearest hit
            if (distance < result.distance) {
                result.hit = true;
                result.particleIndex = i;
                result.hitPoint = hitPoint;
                result.distance = distance;
            }
        }
    }
    
    return result;
}

bool ParticlePicker::raySphereIntersect(
    const Ray& ray,
    const glm::dvec3& center,
    double radius,
    glm::dvec3& hitPoint
) {
    // Ray-sphere intersection using quadratic formula
    glm::dvec3 oc = glm::dvec3(ray.origin) - center;
    
    double a = glm::dot(glm::dvec3(ray.direction), glm::dvec3(ray.direction));  // Should be 1.0 (normalized)
    double b = 2.0 * glm::dot(oc, glm::dvec3(ray.direction));
    double c = glm::dot(oc, oc) - radius * radius;
    
    double discriminant = b * b - 4.0 * a * c;
    
    if (discriminant < 0.0) {
        // No intersection
        return false;
    }
    
    // Calculate intersection point (use nearest intersection)
    double sqrtDiscriminant = std::sqrt(discriminant);
    double t1 = (-b - sqrtDiscriminant) / (2.0 * a);
    double t2 = (-b + sqrtDiscriminant) / (2.0 * a);
    
    // Use positive t value (ray pointing forward)
    double t = (t1 > 0.0) ? t1 : t2;
    
    if (t < 0.0) {
        // Intersection is behind ray origin
        return false;
    }
    
    // Calculate hit point
    hitPoint = glm::dvec3(ray.origin) + t * glm::dvec3(ray.direction);
    
    return true;
}

