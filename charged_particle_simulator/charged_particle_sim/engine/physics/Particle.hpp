#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "engine/core/Constants.hpp"
#include <deque>
#include <chrono>

/**
 * Particle Data Structure
 * 
 * Represents a charged particle with physical properties (charge, mass, position, velocity)
 * and visualization properties (color, visual radius).
 * 
 * All physical properties use SI units:
 * - Position: meters
 * - Velocity: meters/second
 * - Charge: coulombs
 * - Mass: kilograms
 */
struct Particle {
    // === Physical Properties (SI Units) ===
    glm::dvec3 position;      // Position in meters
    glm::dvec3 velocity;       // Velocity in m/s
    glm::dvec3 acceleration;  // Acceleration in m/s² (computed each frame)
    
    double charge;            // Charge in coulombs (e.g., -1.602e-19 for electron)
    double mass;              // Mass in kilograms (e.g., 9.109e-31 for electron)
    
    // === Visualization Properties ===
    float visualRadius;       // Visual radius in meters (scaled for visibility)
    glm::vec3 color;          // RGB color (red = positive, blue = negative)
    
    // === State Flags ===
    bool isBeingDragged;      // True when particle is being dragged by user
    bool isFixed;             // True if particle is immovable (anchor)
    
    // === History (for retarded potentials - Phase 5) ===
    struct HistoryPoint {
        glm::dvec3 position;
        glm::dvec3 velocity;
        double timestamp;
    };
    std::deque<HistoryPoint> history;  // Ring buffer for time-delayed lookups
    static constexpr size_t MAX_HISTORY = 10000;
    
    // === Factory Methods ===
    
    /**
     * Create an electron at the specified position
     * Charge: -1.602176634e-19 C
     * Mass: 9.1093837015e-31 kg
     */
    static Particle createElectron(const glm::dvec3& pos);
    
    /**
     * Create a proton at the specified position
     * Charge: +1.602176634e-19 C
     * Mass: 1.67262192369e-27 kg
     */
    static Particle createProton(const glm::dvec3& pos);
    
    /**
     * Create a custom particle with specified charge and mass
     */
    static Particle createCustom(const glm::dvec3& pos, double q, double m);
    
    // === Utility Methods ===
    
    /**
     * Get charge sign: +1 for positive, -1 for negative, 0 for neutral
     */
    int getChargeSign() const;
    
    /**
     * Update color based on charge sign
     * Red for positive, blue for negative
     */
    void updateColorFromCharge();
    
    /**
     * Record current state to history buffer
     */
    void recordHistory(double timestamp);
    
    /**
     * Clear history buffer
     */
    void clearHistory();
};

