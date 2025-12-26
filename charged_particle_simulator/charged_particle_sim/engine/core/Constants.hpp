#pragma once

/**
 * Physical Constants for Charged Particle Simulation
 * All values in SI units (meters, kilograms, seconds, coulombs)
 */

namespace PhysicsConstants {
    // Speed of light in vacuum (m/s)
    constexpr double c = 299792458.0;
    
    // Coulomb constant: k = 1/(4πε₀) ≈ 8.9875517923e9 N·m²/C²
    // This is the constant in Coulomb's law: F = k * q1 * q2 / r²
    constexpr double k = 8.9875517923e9;
    
    // Permittivity of free space: ε₀ = 8.8541878128e-12 C²/(N·m²)
    constexpr double epsilon_0 = 8.8541878128e-12;
    
    // Permeability of free space: μ₀ = 4π × 10⁻⁷ N/A²
    constexpr double mu_0 = 1.25663706212e-6;
    
    // Elementary charge: e = 1.602176634e-19 C
    // This is the charge of a single electron (negative) or proton (positive)
    constexpr double e = 1.602176634e-19;
    
    // Electron mass: m_e = 9.1093837015e-31 kg
    constexpr double m_e = 9.1093837015e-31;
    
    // Proton mass: m_p = 1.67262192369e-27 kg
    constexpr double m_p = 1.67262192369e-27;
    
    // Mass ratio: proton is approximately 1836 times heavier than electron
    constexpr double m_p_over_m_e = m_p / m_e;  // ≈ 1836.15
    
    // Minimum safe distance for field calculations (meters)
    // Prevents division by zero when evaluating field at charge location
    constexpr double MIN_SAFE_DISTANCE = 1e-15;
    
    // Maximum reasonable velocity (10% of speed of light)
    // Prevents numerical instability from relativistic effects
    constexpr double MAX_VELOCITY = 0.1 * c;
    
    // Maximum reasonable release velocity from dragging (m/s)
    constexpr double MAX_RELEASE_SPEED = 1e6;
}

