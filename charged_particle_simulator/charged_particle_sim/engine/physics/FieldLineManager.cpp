#include "FieldLineManager.hpp"
#include "engine/core/Logger.hpp"
#include <algorithm>
#include <cmath>

FieldLineManager::FieldLineManager()
    : m_dirty(true)
    , m_maxRegenerationRate(10.0)  // 10 Hz default
    , m_lastRegenerationTime(std::chrono::high_resolution_clock::now())
{
}

const std::vector<FieldLine>& FieldLineManager::getFieldLines(
    const std::vector<Particle>& particles,
    const FieldLineConfig& config
) {
    // Check if regeneration is needed
    bool needsRegeneration = m_dirty || 
                            particlesHaveMoved(particles) ||
                            shouldRegenerate();
    
    if (needsRegeneration) {
        LOG_DEBUG("Regenerating field lines...");
        
        // Generate new field lines
        m_cachedLines = FieldLineGenerator::generateAll(particles, config);
        
        // Cache particle positions
        m_lastParticlePositions.clear();
        m_lastParticlePositions.reserve(particles.size());
        for (const auto& p : particles) {
            m_lastParticlePositions.push_back(p.position);
        }
        
        // Mark as clean
        m_dirty = false;
        m_lastRegenerationTime = std::chrono::high_resolution_clock::now();
        
        LOG_DEBUG("Generated " + std::to_string(m_cachedLines.size()) + " field lines");
    }
    
    return m_cachedLines;
}

void FieldLineManager::markDirty() {
    m_dirty = true;
}

bool FieldLineManager::particlesHaveMoved(const std::vector<Particle>& particles) const {
    // Check if number of particles changed
    if (particles.size() != m_lastParticlePositions.size()) {
        return true;
    }
    
    // Check if any particle has moved significantly
    for (size_t i = 0; i < particles.size(); ++i) {
        double distance = glm::length(particles[i].position - m_lastParticlePositions[i]);
        if (distance > MOVEMENT_THRESHOLD) {
            return true;
        }
    }
    
    return false;
}

bool FieldLineManager::shouldRegenerate() const {
    // Throttle regeneration based on max rate
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(now - m_lastRegenerationTime).count();
    double minInterval = 1.0 / m_maxRegenerationRate;
    
    return elapsed >= minInterval;
}

