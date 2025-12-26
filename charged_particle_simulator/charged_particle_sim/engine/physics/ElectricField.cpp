#include "ElectricField.hpp"
#include "engine/core/Logger.hpp"
#include <cmath>

glm::dvec3 ElectricField::fromPointCharge(
    const glm::dvec3& evalPoint,
    const glm::dvec3& chargePos,
    double q
) {
    // Vector from charge to evaluation point
    glm::dvec3 r = evalPoint - chargePos;
    double rMag = glm::length(r);
    
    // Prevent singularity at charge location
    if (rMag < PhysicsConstants::MIN_SAFE_DISTANCE) {
        // Return zero field if too close to charge
        return glm::dvec3(0.0);
    }
    
    // Unit vector in direction from charge to evaluation point
    glm::dvec3 rHat = r / rMag;
    
    // Coulomb's law: E = k * q / r² * r̂
    // Field points away from positive charges, toward negative charges
    double fieldMagnitude = PhysicsConstants::k * q / (rMag * rMag);
    
    return fieldMagnitude * rHat;
}

glm::dvec3 ElectricField::totalField(
    const glm::dvec3& evalPoint,
    const std::vector<Particle>& particles
) {
    glm::dvec3 totalE(0.0);
    
    // Sum contributions from all particles
    for (const auto& particle : particles) {
        // Skip if particle is at evaluation point (self-interaction)
        if (glm::length(evalPoint - particle.position) < PhysicsConstants::MIN_SAFE_DISTANCE) {
            continue;
        }
        
        totalE += fromPointCharge(evalPoint, particle.position, particle.charge);
    }
    
    return totalE;
}

double ElectricField::magnitude(
    const glm::dvec3& evalPoint,
    const std::vector<Particle>& particles
) {
    glm::dvec3 E = totalField(evalPoint, particles);
    return glm::length(E);
}

glm::dvec3 ElectricField::direction(
    const glm::dvec3& evalPoint,
    const std::vector<Particle>& particles
) {
    glm::dvec3 E = totalField(evalPoint, particles);
    double EMag = glm::length(E);
    
    // Return zero vector if field is too weak
    if (EMag < 1e-20) {
        return glm::dvec3(0.0);
    }
    
    return glm::normalize(E);
}

glm::dvec3 ElectricField::retardedField(
    const glm::dvec3& evalPoint,
    double currentTime,
    const Particle& source
) {
    // Calculate distance from source to evaluation point
    double distance = glm::length(evalPoint - source.position);
    
    // Calculate retarded time: t_ret = t - |r|/c
    double retardedTime = currentTime - distance / PhysicsConstants::c;
    
    // Look up particle position at retarded time from history
    glm::dvec3 retardedPosition = source.position;
    
    if (!source.history.empty()) {
        // Find closest history point to retarded time
        double minTimeDiff = std::numeric_limits<double>::max();
        int bestIndex = -1;
        
        for (size_t i = 0; i < source.history.size(); ++i) {
            double timeDiff = std::abs(source.history[i].timestamp - retardedTime);
            if (timeDiff < minTimeDiff) {
                minTimeDiff = timeDiff;
                bestIndex = static_cast<int>(i);
            }
        }
        
        if (bestIndex >= 0) {
            // Use exact match if found, otherwise interpolate
            if (minTimeDiff < 1e-9) {
                retardedPosition = source.history[bestIndex].position;
            } else if (bestIndex > 0 && bestIndex < static_cast<int>(source.history.size()) - 1) {
                // Linear interpolation between adjacent history points
                const auto& p1 = source.history[bestIndex - 1];
                const auto& p2 = source.history[bestIndex + 1];
                
                double t1 = p1.timestamp;
                double t2 = p2.timestamp;
                
                if (std::abs(t2 - t1) > 1e-9) {
                    double alpha = (retardedTime - t1) / (t2 - t1);
                    retardedPosition = p1.position + alpha * (p2.position - p1.position);
                } else {
                    retardedPosition = source.history[bestIndex].position;
                }
            } else {
                retardedPosition = source.history[bestIndex].position;
            }
        }
    }
    
    // Calculate field from retarded position
    return fromPointCharge(evalPoint, retardedPosition, source.charge);
}

