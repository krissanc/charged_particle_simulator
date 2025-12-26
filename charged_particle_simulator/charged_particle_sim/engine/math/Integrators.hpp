#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <vector>

/**
 * Numerical Integrators
 * 
 * Provides various numerical integration methods for solving differential equations.
 * Used for field line generation and particle dynamics.
 */

/**
 * Runge-Kutta 4th order (RK4) integrator
 * 
 * Solves: dy/dt = f(t, y)
 * 
 * @param y Current state vector
 * @param t Current time
 * @param h Step size
 * @param f Derivative function: f(t, y) -> dy/dt
 * @return New state after step
 */
template<typename State, typename Derivative>
State rk4Step(
    const State& y,
    double t,
    double h,
    std::function<Derivative(const State&, double)> f
) {
    // k1 = f(t, y)
    Derivative k1 = f(y, t);
    
    // k2 = f(t + h/2, y + h*k1/2)
    State y2 = y + static_cast<State>(h * 0.5 * k1);
    Derivative k2 = f(y2, t + h * 0.5);
    
    // k3 = f(t + h/2, y + h*k2/2)
    State y3 = y + static_cast<State>(h * 0.5 * k2);
    Derivative k3 = f(y3, t + h * 0.5);
    
    // k4 = f(t + h, y + h*k3)
    State y4 = y + static_cast<State>(h * k3);
    Derivative k4 = f(y4, t + h);
    
    // y_new = y + (h/6) * (k1 + 2*k2 + 2*k3 + k4)
    return y + static_cast<State>((h / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4));
}

/**
 * Velocity Verlet integrator for particle dynamics
 * 
 * More stable than Euler for long-term simulations.
 * 
 * @param position Current position
 * @param velocity Current velocity
 * @param acceleration Current acceleration
 * @param dt Time step
 * @return New position and velocity
 */
struct VerletResult {
    glm::dvec3 position;
    glm::dvec3 velocity;
};

inline VerletResult verletStep(
    const glm::dvec3& position,
    const glm::dvec3& velocity,
    const glm::dvec3& acceleration,
    double dt
) {
    VerletResult result;
    
    // v_new = v + a * dt
    result.velocity = velocity + acceleration * dt;
    
    // x_new = x + v * dt + 0.5 * a * dt²
    result.position = position + velocity * dt + 0.5 * acceleration * dt * dt;
    
    return result;
}

/**
 * Semi-implicit Euler integrator (simpler alternative)
 * 
 * @param position Current position
 * @param velocity Current velocity
 * @param acceleration Current acceleration
 * @param dt Time step
 * @return New position and velocity
 */
struct EulerResult {
    glm::dvec3 position;
    glm::dvec3 velocity;
};

inline EulerResult eulerStep(
    const glm::dvec3& position,
    const glm::dvec3& velocity,
    const glm::dvec3& acceleration,
    double dt
) {
    EulerResult result;
    
    // v_new = v + a * dt
    result.velocity = velocity + acceleration * dt;
    
    // x_new = x + v_new * dt
    result.position = position + result.velocity * dt;
    
    return result;
}

