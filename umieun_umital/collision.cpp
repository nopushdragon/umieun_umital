#include "collision.h"

bool is_separated(const OBB& a, const OBB& b, const glm::vec3& axis) {
    if (glm::length(axis) < 1e-6) return false;

    glm::vec3 T = b.center - a.center;

    float distance_proj = glm::abs(glm::dot(T, axis));

    float radius_a =
        glm::abs(glm::dot(a.half_length.x * a.u[0], axis)) +
        glm::abs(glm::dot(a.half_length.y * a.u[1], axis)) +
        glm::abs(glm::dot(a.half_length.z * a.u[2], axis));

    float radius_b =
        glm::abs(glm::dot(b.half_length.x * b.u[0], axis)) +
        glm::abs(glm::dot(b.half_length.y * b.u[1], axis)) +
        glm::abs(glm::dot(b.half_length.z * b.u[2], axis));

    return distance_proj > (radius_a + radius_b);
}

bool check_collision(const OBB& obbA, const OBB& obbB) {
    const OBB& a = obbA;
    const OBB& b = obbB;

    for (int i = 0; i < 3; i++) {
        if (is_separated(a, b, a.u[i])) return false;
    }

    for (int i = 0; i < 3; i++) {
        if (is_separated(a, b, b.u[i])) return false;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glm::vec3 cross_axis = glm::cross(a.u[i], b.u[j]);
            if (is_separated(a, b, cross_axis)) return false;
        }
    }

    return true;
}