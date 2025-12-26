# Architecture Documentation

## System Overview

The Charged Particle Simulator follows a modular, physics-first architecture with strict separation of concerns.

## Module Structure

### Core Engine (`engine/core/`)

**Logger**: Hierarchical logging system with file and console output
- Log levels: DEBUG, INFO, WARN, ERROR
- Thread-safe with mutex protection
- Timestamp formatting

**Constants**: Physical constants in SI units
- Speed of light, Coulomb constant, elementary charge, etc.

**InputManager**: Centralized input handling
- Wires GLFW callbacks to camera, drag controller, etc.

### Physics Engine (`engine/physics/`)

**Particle**: Charged particle data structure
- Physical properties (position, velocity, charge, mass)
- Visualization properties (color, visual radius)
- History buffer for retarded potentials

**ElectricField**: Electric field computation
- Coulomb's law implementation
- Total field from multiple charges
- Retarded field calculation (Phase 5)

**FieldLineGenerator**: Field line generation
- Seed point distribution (Fibonacci sphere)
- RK4 integration along field direction
- Termination conditions

**FieldLineManager**: Field line caching and management
- Dirty flag system
- Throttled regeneration (10 Hz default)

### Rendering (`engine/render/`)

**ParticleRenderer**: Particle sphere rendering
- UV sphere mesh generation
- Instanced rendering for multiple particles
- Color based on charge sign

**FieldLineRenderer**: Field line rendering
- Polyline rendering with GL_LINE_STRIP
- Color gradient based on field magnitude
- Animated stripe pattern for direction

### Interaction (`engine/interaction/`)

**Camera**: Orbit camera system
- Spherical coordinates (azimuth, elevation, radius)
- Mouse drag for rotation, scroll for zoom

**RayCaster**: Screen-to-world ray conversion
- Unprojects mouse coordinates to 3D rays

**ParticlePicker**: Ray-sphere intersection
- Picks nearest particle under mouse cursor

**DragController**: Particle dragging
- Drag plane projection
- Velocity inference from motion history

### Scene Management (`engine/scene/`)

**ParticleSystem**: Particle collection and simulation
- Force calculation
- Time integration (Verlet or Euler)
- Collision prevention

### Math (`engine/math/`)

**Integrators**: Numerical integration methods
- RK4 for field lines
- Velocity Verlet for particle dynamics
- Semi-implicit Euler (alternative)

## Data Flow

```
User Input → InputManager → Camera/DragController
                                    ↓
                            ParticleSystem (updates positions)
                                    ↓
                            FieldLineManager (regenerates if needed)
                                    ↓
                            Renderer (draws particles and field lines)
```

## Design Principles

1. **Physics-First**: All physics calculations use proper SI units
2. **Separation of Concerns**: No OpenGL calls in physics code
3. **Modularity**: Each module has a single responsibility
4. **Extensibility**: Easy to add new features (magnetic fields, etc.)

## Performance Considerations

- Field line generation is CPU-bound and throttled
- Particle rendering uses instanced rendering for efficiency
- Field line caching reduces regeneration frequency
- GPU acceleration planned for Phase 6

## Future Extensions

- Magnetic field support (Biot-Savart law)
- Full Lorentz force (F = q(E + v×B))
- Radiation effects
- GPU-accelerated field line generation
- Educational measurement tools

