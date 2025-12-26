#include <cassert>
#include <cmath>
#include <iostream>
#include "engine/physics/ElectricField.hpp"
#include "engine/core/Constants.hpp"

/**
 * Unit tests for Coulomb's law and electric field calculations
 */

void testSinglePointCharge() {
    std::cout << "Testing single point charge..." << std::endl;
    
    // Test: 1 Coulomb charge at origin, evaluate field at (1, 0, 0)
    glm::dvec3 chargePos(0.0, 0.0, 0.0);
    glm::dvec3 evalPoint(1.0, 0.0, 0.0);
    double q = 1.0; // 1 Coulomb
    
    glm::dvec3 E = ElectricField::fromPointCharge(evalPoint, chargePos, q);
    
    // Expected: E = k * q / r² = 8.99e9 * 1 / 1² = 8.99e9 N/C
    double expectedMagnitude = PhysicsConstants::k * q / (1.0 * 1.0);
    double actualMagnitude = glm::length(E);
    
    // Field should point away from positive charge (in +X direction)
    assert(E.x > 0 && std::abs(E.y) < 1e-6 && std::abs(E.z) < 1e-6);
    
    // Check magnitude (allow 0.1% tolerance)
    double tolerance = expectedMagnitude * 0.001;
    assert(std::abs(actualMagnitude - expectedMagnitude) < tolerance);
    
    std::cout << "  ✓ Single point charge test passed" << std::endl;
}

void testNegativeCharge() {
    std::cout << "Testing negative charge..." << std::endl;
    
    // Test: -1 Coulomb charge at origin, evaluate field at (1, 0, 0)
    glm::dvec3 chargePos(0.0, 0.0, 0.0);
    glm::dvec3 evalPoint(1.0, 0.0, 0.0);
    double q = -1.0; // -1 Coulomb
    
    glm::dvec3 E = ElectricField::fromPointCharge(evalPoint, chargePos, q);
    
    // Field should point toward negative charge (in -X direction)
    assert(E.x < 0 && std::abs(E.y) < 1e-6 && std::abs(E.z) < 1e-6);
    
    std::cout << "  ✓ Negative charge test passed" << std::endl;
}

void testNearZeroDistance() {
    std::cout << "Testing near-zero distance handling..." << std::endl;
    
    // Test: Field at charge location should return zero (not infinity)
    glm::dvec3 chargePos(0.0, 0.0, 0.0);
    glm::dvec3 evalPoint(0.0, 0.0, 0.0);
    double q = 1.0;
    
    glm::dvec3 E = ElectricField::fromPointCharge(evalPoint, chargePos, q);
    
    // Should return zero vector, not crash
    assert(glm::length(E) < 1e-10);
    
    std::cout << "  ✓ Near-zero distance test passed" << std::endl;
}

void testTotalField() {
    std::cout << "Testing total field from multiple charges..." << std::endl;
    
    // Create two opposite charges
    std::vector<Particle> particles;
    particles.push_back(Particle::createElectron(glm::dvec3(-1.0, 0.0, 0.0)));
    particles.push_back(Particle::createProton(glm::dvec3(1.0, 0.0, 0.0)));
    
    // Evaluate field at origin
    glm::dvec3 evalPoint(0.0, 0.0, 0.0);
    glm::dvec3 E = ElectricField::totalField(evalPoint, particles);
    
    // At origin, fields from both charges should point in same direction
    // (toward electron, away from proton)
    // Result should be non-zero
    assert(glm::length(E) > 1e-10);
    
    std::cout << "  ✓ Total field test passed" << std::endl;
}

int main() {
    std::cout << "Running Coulomb's law unit tests..." << std::endl;
    std::cout << std::endl;
    
    try {
        testSinglePointCharge();
        testNegativeCharge();
        testNearZeroDistance();
        testTotalField();
        
        std::cout << std::endl;
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}

