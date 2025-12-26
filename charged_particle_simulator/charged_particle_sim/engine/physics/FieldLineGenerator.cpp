#include "FieldLineGenerator.hpp"
#include "engine/core/Logger.hpp"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Golden angle for Fibonacci sphere distribution
const double GOLDEN_ANGLE = M_PI * (3.0 - sqrt(5.0));

FieldLine FieldLineGenerator::generate(
    const glm::dvec3& seedPoint,
    const std::vector<Particle>& particles,
    const FieldLineConfig& config,
    bool traceForward
) {
    FieldLine line;
    line.isForward = traceForward;
    
    glm::dvec3 pos = seedPoint;
    double h = config.stepSize;
    
    // Determine source charge (nearest particle)
    if (!particles.empty()) {
        double minDist = std::numeric_limits<double>::max();
        for (const auto& p : particles) {
            double dist = glm::length(pos - p.position);
            if (dist < minDist) {
                minDist = dist;
                line.sourceCharge = static_cast<float>(p.charge);
            }
        }
    }
    
    for (int step = 0; step < config.maxStepsPerLine; ++step) {
        // Evaluate electric field at current position
        glm::dvec3 E = ElectricField::totalField(pos, particles);
        double EMag = glm::length(E);
        
        // === Termination Conditions ===
        
        // 1. Field magnitude too weak
        if (EMag < config.minFieldMagnitude) {
            line.isComplete = true;
            break;
        }
        
        // 2. Distance from origin too large
        if (glm::length(pos) > config.maxDistance) {
            line.isComplete = true;
            break;
        }
        
        // 3. Entered a particle (opposite sign charge)
        int particleIndex = -1;
        if (isInsideParticle(pos, particles, particleIndex)) {
            // Check if charge sign matches
            if (particleIndex >= 0 && particleIndex < static_cast<int>(particles.size())) {
                const Particle& p = particles[particleIndex];
                int sourceSign = (line.sourceCharge > 0) ? 1 : -1;
                int targetSign = (p.charge > 0) ? 1 : -1;
                
                // Terminate if opposite sign (field line ends at opposite charge)
                if (sourceSign != targetSign) {
                    line.isComplete = true;
                    break;
                }
            }
        }
        
        // === Store Point ===
        line.points.push_back(pos);
        line.fieldMagnitudes.push_back(static_cast<float>(EMag));
        
        // === RK4 Integration ===
        
        // Field direction (normalized)
        glm::dvec3 direction = traceForward ? glm::normalize(E) : -glm::normalize(E);
        
        // Adaptive step sizing based on curvature
        double adaptiveH = h;
        if (config.useAdaptiveStep) {
            double curvature = estimateCurvature(pos, direction, particles);
            adaptiveH = h / (1.0 + curvature * config.adaptiveStepFactor);
        }
        
        // RK4 step: integrate along field direction
        // dy/dt = direction (field direction is constant for small steps)
        // We can use a simplified RK4 for this
        glm::dvec3 k1 = direction;
        glm::dvec3 k2 = ElectricField::direction(pos + 0.5 * adaptiveH * k1, particles);
        if (glm::length(k2) < 1e-10) k2 = k1; // Fallback if field too weak
        if (!traceForward) k2 = -k2;
        
        glm::dvec3 k3 = ElectricField::direction(pos + 0.5 * adaptiveH * k2, particles);
        if (glm::length(k3) < 1e-10) k3 = k2;
        if (!traceForward) k3 = -k3;
        
        glm::dvec3 k4 = ElectricField::direction(pos + adaptiveH * k3, particles);
        if (glm::length(k4) < 1e-10) k4 = k3;
        if (!traceForward) k4 = -k4;
        
        // Update position using RK4
        pos = pos + (adaptiveH / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
    }
    
    if (!line.isComplete && line.points.size() >= static_cast<size_t>(config.maxStepsPerLine)) {
        // Reached maximum steps
        line.isComplete = true;
    }
    
    return line;
}

std::vector<glm::dvec3> FieldLineGenerator::generateSeedPoints(
    const Particle& source,
    int count
) {
    std::vector<glm::dvec3> seedPoints;
    seedPoints.reserve(count);
    
    // Fibonacci sphere algorithm for uniform distribution
    for (int i = 0; i < count; ++i) {
        double theta = GOLDEN_ANGLE * i;
        double y = 1.0 - (2.0 * i) / (count - 1.0); // y goes from 1 to -1
        double radius = sqrt(1.0 - y * y);
        
        double x = cos(theta) * radius;
        double z = sin(theta) * radius;
        
        // Scale by particle visual radius and offset by particle position
        // Cast visualRadius (float) to double for dvec3 multiplication
        glm::dvec3 point = source.position + static_cast<double>(source.visualRadius) * glm::dvec3(x, y, z);
        seedPoints.push_back(point);
    }
    
    return seedPoints;
}

std::vector<FieldLine> FieldLineGenerator::generateAll(
    const std::vector<Particle>& particles,
    const FieldLineConfig& config
) {
    std::vector<FieldLine> allLines;
    
    for (const auto& particle : particles) {
        // Generate seed points around this particle
        std::vector<glm::dvec3> seedPoints = generateSeedPoints(particle, config.seedPointsPerParticle);
        
        // Generate field line from each seed point
        for (const auto& seed : seedPoints) {
            // Trace forward (away from positive, toward negative)
            FieldLine line = generate(seed, particles, config, true);
            if (line.points.size() > 1) { // Only add if line has points
                allLines.push_back(line);
            }
        }
    }
    
    return allLines;
}

double FieldLineGenerator::estimateCurvature(
    const glm::dvec3& pos,
    const glm::dvec3& dir,
    const std::vector<Particle>& particles
) {
    // Estimate curvature by checking how much direction changes over small step
    double eps = 0.01;
    glm::dvec3 E_ahead = ElectricField::totalField(pos + eps * dir, particles);
    double EMag_ahead = glm::length(E_ahead);
    
    if (EMag_ahead < 1e-20) {
        return 0.0; // Field too weak, no curvature
    }
    
    glm::dvec3 dir_ahead = glm::normalize(E_ahead);
    double curvature = glm::length(dir_ahead - dir) / eps;
    
    return curvature;
}

bool FieldLineGenerator::isInsideParticle(
    const glm::dvec3& point,
    const std::vector<Particle>& particles,
    int& particleIndex
) {
    particleIndex = -1;
    
    for (size_t i = 0; i < particles.size(); ++i) {
        double dist = glm::length(point - particles[i].position);
        if (dist < particles[i].visualRadius * 0.5) {
            particleIndex = static_cast<int>(i);
            return true;
        }
    }
    
    return false;
}

