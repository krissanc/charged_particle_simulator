#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Particle.hpp"
#include "ElectricField.hpp"
#include "engine/math/Integrators.hpp"

/**
 * Field Line Data Structure
 * 
 * Represents a single field line as a polyline in 3D space.
 */
struct FieldLine {
    std::vector<glm::dvec3> points;           // 3D polyline points
    std::vector<float> fieldMagnitudes;       // |E| at each point (for color mapping)
    float sourceCharge;                       // Charge of source particle
    bool isComplete;                          // True if line reached termination condition
    bool isForward;                           // True if traced forward, false if backward
    
    FieldLine() : isComplete(false), isForward(true), sourceCharge(0.0f) {}
};

/**
 * Field Line Generation Configuration
 */
struct FieldLineConfig {
    int seedPointsPerParticle = 24;           // Number of seed points distributed on sphere
    int maxStepsPerLine = 1000;               // Maximum integration steps per line
    double stepSize = 0.01;                   // Base integration step size
    double minFieldMagnitude = 1e-6;          // Stop if field magnitude below this
    double maxDistance = 100.0;               // Stop if distance from origin exceeds this
    bool useAdaptiveStep = true;              // Use adaptive step sizing
    double adaptiveStepFactor = 10.0;         // Factor for adaptive step adjustment
};

/**
 * Field Line Generator
 * 
 * Generates electric field lines by numerically integrating along field direction.
 * Uses RK4 integration to trace field lines from seed points around particles.
 */
class FieldLineGenerator {
public:
    /**
     * Generate a single field line starting from a seed point
     * 
     * @param seedPoint Starting point for field line
     * @param particles All charged particles in simulation
     * @param config Configuration for field line generation
     * @param traceForward If true, trace in direction of field; if false, trace opposite
     * @return Generated field line
     */
    static FieldLine generate(
        const glm::dvec3& seedPoint,
        const std::vector<Particle>& particles,
        const FieldLineConfig& config,
        bool traceForward = true
    );
    
    /**
     * Generate seed points distributed evenly on a sphere around a particle
     * Uses Fibonacci sphere algorithm for uniform distribution
     * 
     * @param source Source particle
     * @param count Number of seed points to generate
     * @return Vector of seed point positions
     */
    static std::vector<glm::dvec3> generateSeedPoints(
        const Particle& source,
        int count
    );
    
    /**
     * Generate all field lines for all particles
     * 
     * @param particles All charged particles
     * @param config Configuration for field line generation
     * @return Vector of all generated field lines
     */
    static std::vector<FieldLine> generateAll(
        const std::vector<Particle>& particles,
        const FieldLineConfig& config
    );

private:
    /**
     * Estimate curvature at a point to adjust step size
     */
    static double estimateCurvature(
        const glm::dvec3& pos,
        const glm::dvec3& dir,
        const std::vector<Particle>& particles
    );
    
    /**
     * Check if point is inside any particle
     */
    static bool isInsideParticle(
        const glm::dvec3& point,
        const std::vector<Particle>& particles,
        int& particleIndex
    );
};

