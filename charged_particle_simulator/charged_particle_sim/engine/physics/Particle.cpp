#include "Particle.hpp"
#include "engine/core/Logger.hpp"
#include <cmath>

Particle Particle::createElectron(const glm::dvec3& pos) {
    Particle p;
    p.position = pos;
    p.velocity = glm::dvec3(0.0);
    p.acceleration = glm::dvec3(0.0);
    p.charge = -PhysicsConstants::e;  // Negative charge
    p.mass = PhysicsConstants::m_e;     // Electron mass
    p.visualRadius = 1e-10f;            // Visual radius (scaled)
    p.isBeingDragged = false;
    p.isFixed = false;
    p.updateColorFromCharge();
    
    LOG_DEBUG("Created electron at (" + 
              std::to_string(pos.x) + ", " +
              std::to_string(pos.y) + ", " +
              std::to_string(pos.z) + ")");
    
    return p;
}

Particle Particle::createProton(const glm::dvec3& pos) {
    Particle p;
    p.position = pos;
    p.velocity = glm::dvec3(0.0);
    p.acceleration = glm::dvec3(0.0);
    p.charge = PhysicsConstants::e;     // Positive charge
    p.mass = PhysicsConstants::m_p;     // Proton mass
    p.visualRadius = 1e-10f;            // Visual radius (scaled)
    p.isBeingDragged = false;
    p.isFixed = false;
    p.updateColorFromCharge();
    
    LOG_DEBUG("Created proton at (" + 
              std::to_string(pos.x) + ", " +
              std::to_string(pos.y) + ", " +
              std::to_string(pos.z) + ")");
    
    return p;
}

Particle Particle::createCustom(const glm::dvec3& pos, double q, double m) {
    Particle p;
    p.position = pos;
    p.velocity = glm::dvec3(0.0);
    p.acceleration = glm::dvec3(0.0);
    p.charge = q;
    p.mass = m;
    p.visualRadius = 1e-10f;            // Default visual radius
    p.isBeingDragged = false;
    p.isFixed = false;
    p.updateColorFromCharge();
    
    LOG_DEBUG("Created custom particle: q=" + std::to_string(q) + 
              " C, m=" + std::to_string(m) + " kg at (" +
              std::to_string(pos.x) + ", " +
              std::to_string(pos.y) + ", " +
              std::to_string(pos.z) + ")");
    
    return p;
}

int Particle::getChargeSign() const {
    if (charge > 1e-20) return 1;   // Positive
    if (charge < -1e-20) return -1; // Negative
    return 0;                       // Neutral (within tolerance)
}

void Particle::updateColorFromCharge() {
    int sign = getChargeSign();
    if (sign > 0) {
        // Positive charge: red
        color = glm::vec3(1.0f, 0.3f, 0.2f);
    } else if (sign < 0) {
        // Negative charge: blue
        color = glm::vec3(0.2f, 0.5f, 1.0f);
    } else {
        // Neutral: gray
        color = glm::vec3(0.5f, 0.5f, 0.5f);
    }
}

void Particle::recordHistory(double timestamp) {
    HistoryPoint point;
    point.position = position;
    point.velocity = velocity;
    point.timestamp = timestamp;
    
    history.push_back(point);
    
    // Maintain maximum history size
    if (history.size() > MAX_HISTORY) {
        history.pop_front();
    }
}

void Particle::clearHistory() {
    history.clear();
}

