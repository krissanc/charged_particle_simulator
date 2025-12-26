#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Particle.hpp"
#include "engine/core/Constants.hpp"

/**
 * Electric Field Calculator
 * 
 * Computes electric fields using Coulomb's law:
 * E = k * q / r² * r̂
 * 
 * Where:
 * - k = 1/(4πε₀) ≈ 8.99e9 N·m²/C² (Coulomb constant)
 * - q = charge in coulombs
 * - r = distance from charge to evaluation point
 * - r̂ = unit vector from charge to evaluation point
 */
class ElectricField {
public:
    /**
     * Compute electric field at point p from a single point charge
     * 
     * @param evalPoint Point where field is evaluated (meters)
     * @param chargePos Position of the charge (meters)
     * @param q Charge magnitude in coulombs
     * @return Electric field vector in N/C (volts/meter)
     */
    static glm::dvec3 fromPointCharge(
        const glm::dvec3& evalPoint,
        const glm::dvec3& chargePos,
        double q
    );
    
    /**
     * Compute total electric field at point p from all particles
     * 
     * @param evalPoint Point where field is evaluated (meters)
     * @param particles Vector of all charged particles
     * @return Total electric field vector in N/C
     */
    static glm::dvec3 totalField(
        const glm::dvec3& evalPoint,
        const std::vector<Particle>& particles
    );
    
    /**
     * Compute electric field magnitude at point p
     * 
     * @param evalPoint Point where field is evaluated (meters)
     * @param particles Vector of all charged particles
     * @return Field magnitude in N/C
     */
    static double magnitude(
        const glm::dvec3& evalPoint,
        const std::vector<Particle>& particles
    );
    
    /**
     * Get field direction (unit vector) at point p
     * 
     * @param evalPoint Point where field is evaluated (meters)
     * @param particles Vector of all charged particles
     * @return Unit vector in direction of field, or zero vector if field is too weak
     */
    static glm::dvec3 direction(
        const glm::dvec3& evalPoint,
        const std::vector<Particle>& particles
    );
    
    /**
     * Future: Compute retarded (time-delayed) electric field
     * Accounts for finite speed of light propagation
     * 
     * @param evalPoint Point where field is evaluated (meters)
     * @param currentTime Current simulation time (seconds)
     * @param source Source particle
     * @return Retarded electric field vector
     */
    static glm::dvec3 retardedField(
        const glm::dvec3& evalPoint,
        double currentTime,
        const Particle& source
    );

private:
    // Prevent instantiation (static class)
    ElectricField() = delete;
};

