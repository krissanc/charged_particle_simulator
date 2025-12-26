#include "DragController.hpp"
#include "RayCaster.hpp"
#include "engine/core/Logger.hpp"
#include "engine/core/Constants.hpp"
#include <algorithm>
#include <cmath>

DragController::DragController()
    : m_state(State::IDLE)
    , m_particle(nullptr)
    , m_dragPlaneNormal(0.0, 1.0, 0.0)
    , m_dragPlanePoint(0.0)
    , m_cameraPosition(0.0)
    , m_maxReleaseSpeed(PhysicsConstants::MAX_RELEASE_SPEED)
{
}

void DragController::beginDrag(Particle& particle, const glm::dvec3& hitPoint, const glm::dvec3& cameraPos) {
    m_state = State::DRAGGING;
    m_particle = &particle;
    m_cameraPosition = cameraPos;
    
    // Calculate drag plane normal (perpendicular to camera-particle vector)
    glm::dvec3 toParticle = hitPoint - cameraPos;
    double dist = glm::length(toParticle);
    if (dist > 1e-10) {
        m_dragPlaneNormal = glm::normalize(toParticle);
    } else {
        // Fallback: use Y-up plane
        m_dragPlaneNormal = glm::dvec3(0.0, 1.0, 0.0);
    }
    m_dragPlanePoint = hitPoint;
    
    // Clear motion history
    m_positionHistory.clear();
    
    // Mark particle as being dragged
    particle.isBeingDragged = true;
    
    LOG_DEBUG("Drag started on particle");
}

void DragController::updateDrag(const Ray& ray) {
    if (m_state != State::DRAGGING || !m_particle) {
        return;
    }
    
    // Ray-plane intersection
    glm::dvec3 intersection;
    if (rayPlaneIntersect(ray, m_dragPlanePoint, m_dragPlaneNormal, intersection)) {
        // Update particle position
        m_particle->position = intersection;
        m_particle->velocity = glm::dvec3(0.0);  // Zero velocity while dragging
        
        // Record position in history
        MotionPoint point;
        point.position = intersection;
        point.timestamp = getCurrentTime();
        
        m_positionHistory.push_back(point);
        
        // Maintain maximum history size
        if (m_positionHistory.size() > MAX_HISTORY) {
            m_positionHistory.erase(m_positionHistory.begin());
        }
    }
}

void DragController::endDrag() {
    if (m_state != State::DRAGGING || !m_particle) {
        return;
    }
    
    // Infer release velocity from motion history
    if (m_positionHistory.size() >= 2) {
        glm::dvec3 velocity = inferVelocity();
        
        // Cap velocity to maximum
        double speed = glm::length(velocity);
        if (speed > m_maxReleaseSpeed) {
            velocity = glm::normalize(velocity) * m_maxReleaseSpeed;
        }
        
        m_particle->velocity = velocity;
        
        LOG_DEBUG("Drag ended, inferred velocity: (" +
                  std::to_string(velocity.x) + ", " +
                  std::to_string(velocity.y) + ", " +
                  std::to_string(velocity.z) + ")");
    }
    
    // Mark particle as no longer being dragged
    m_particle->isBeingDragged = false;
    m_particle = nullptr;
    m_state = State::IDLE;
    
    // Clear history
    m_positionHistory.clear();
}

double DragController::getCurrentTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

glm::dvec3 DragController::inferVelocity() const {
    if (m_positionHistory.size() < 2) {
        return glm::dvec3(0.0);
    }
    
    // Calculate velocity from first and last position
    const auto& first = m_positionHistory.front();
    const auto& last = m_positionHistory.back();
    
    double dt = last.timestamp - first.timestamp;
    if (dt < 1e-6) {
        return glm::dvec3(0.0);
    }
    
    glm::dvec3 dp = last.position - first.position;
    return dp / dt;
}

bool DragController::rayPlaneIntersect(
    const Ray& ray,
    const glm::dvec3& planePoint,
    const glm::dvec3& planeNormal,
    glm::dvec3& intersection
) const {
    glm::dvec3 rayOrigin = glm::dvec3(ray.origin);
    glm::dvec3 rayDir = glm::dvec3(ray.direction);
    
    // Ray-plane intersection: t = dot(planePoint - rayOrigin, planeNormal) / dot(rayDir, planeNormal)
    double denom = glm::dot(rayDir, planeNormal);
    
    // Check if ray is parallel to plane
    if (std::abs(denom) < 1e-10) {
        // Fallback: project onto XZ plane at particle's Y
        if (m_particle) {
            intersection = rayOrigin + rayDir * ((m_particle->position.y - rayOrigin.y) / rayDir.y);
            return true;
        }
        return false;
    }
    
    double t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
    
    if (t < 0.0) {
        // Intersection is behind ray origin
        return false;
    }
    
    intersection = rayOrigin + t * rayDir;
    return true;
}

