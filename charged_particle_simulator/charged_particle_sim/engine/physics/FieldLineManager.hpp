#pragma once

#include <vector>
#include <chrono>
#include "FieldLineGenerator.hpp"
#include "Particle.hpp"

/**
 * Field Line Manager
 * 
 * Manages field line generation and caching.
 * Implements dirty flag system and throttling to avoid regenerating lines every frame.
 */
class FieldLineManager {
public:
    FieldLineManager();
    
    /**
     * Get current field lines (regenerates if dirty)
     * 
     * @param particles Current particle positions
     * @param config Field line generation configuration
     * @return Vector of all field lines
     */
    const std::vector<FieldLine>& getFieldLines(
        const std::vector<Particle>& particles,
        const FieldLineConfig& config
    );
    
    /**
     * Mark field lines as dirty (force regeneration on next getFieldLines call)
     */
    void markDirty();
    
    /**
     * Check if field lines are dirty
     */
    bool isDirty() const { return m_dirty; }
    
    /**
     * Set maximum regeneration rate (Hz)
     */
    void setMaxRegenerationRate(double rate) { m_maxRegenerationRate = rate; }
    
    /**
     * Get maximum regeneration rate (Hz)
     */
    double getMaxRegenerationRate() const { return m_maxRegenerationRate; }

private:
    // Cached field lines
    std::vector<FieldLine> m_cachedLines;
    
    // Cached particle positions (for dirty checking)
    std::vector<glm::dvec3> m_lastParticlePositions;
    
    // State flags
    bool m_dirty;
    double m_maxRegenerationRate;  // Maximum regeneration rate in Hz
    std::chrono::high_resolution_clock::time_point m_lastRegenerationTime;
    
    /**
     * Check if particles have moved significantly
     */
    bool particlesHaveMoved(const std::vector<Particle>& particles) const;
    
    /**
     * Check if enough time has passed since last regeneration (throttling)
     */
    bool shouldRegenerate() const;
    
    /**
     * Threshold for considering a particle "moved" (meters)
     */
    static constexpr double MOVEMENT_THRESHOLD = 1e-12;
};

