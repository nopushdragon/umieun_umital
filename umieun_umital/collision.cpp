#include "collision.h"

void update_world_obb(MazeBlockInstance& block) {
	// 모델 매트릭스에서 회전 및 스케일 부분만 추출
    glm::mat3 rotation_scale_mat = glm::mat3(block.modelMatrix);

    //=========================================
	// road_local_obb를 road_world_obb로 변환
    glm::vec4 road_local_center_h = glm::vec4(block.modelPtr->road_local_obb.center, 1.0f);
    block.modelPtr->road_world_obb.center = glm::vec3(block.modelMatrix * road_local_center_h);

    for (int i = 0; i < 3; i++) {
        block.modelPtr->road_world_obb.u[i] = glm::normalize(rotation_scale_mat * block.modelPtr->road_local_obb.u[i]);
    }

    glm::vec3 road_scale_factors = glm::vec3(
        glm::length(rotation_scale_mat[0]), // X축 스케일
        glm::length(rotation_scale_mat[1]), // Y축 스케일
        glm::length(rotation_scale_mat[2])  // Z축 스케일
    );

    block.modelPtr->road_world_obb.half_length = block.modelPtr->road_local_obb.half_length * road_scale_factors;
	//=========================================
    for (int i = 0; i < block.modelPtr->obstacle_local_obb.size();i++) {
        glm::vec4 obstacle_local_center_h = glm::vec4(block.modelPtr->obstacle_local_obb[i].center, 1.0f);
        block.modelPtr->obstacle_world_obb[i].center = glm::vec3(block.modelMatrix * obstacle_local_center_h);

        for (int j = 0; j < 3; j++) {
            block.modelPtr->obstacle_world_obb[i].u[j] = glm::normalize(rotation_scale_mat * block.modelPtr->obstacle_local_obb[i].u[j]);
        }

        glm::vec3 obstacle_scale_factors = glm::vec3(
            glm::length(rotation_scale_mat[0]), // X축 스케일
            glm::length(rotation_scale_mat[1]), // Y축 스케일
            glm::length(rotation_scale_mat[2])  // Z축 스케일
        );

        block.modelPtr->obstacle_world_obb[i].half_length = block.modelPtr->obstacle_local_obb[i].half_length * obstacle_scale_factors;
    }
}

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

bool road_check_collision(const MazeBlockInstance& blockA, const MazeBlockInstance& blockB) {
    const OBB& a = blockA.modelPtr->road_world_obb;
    const OBB& b = blockB.modelPtr->road_world_obb;

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