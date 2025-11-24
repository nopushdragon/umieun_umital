#include "collision.h"

void maze_update_world_obb(MazeBlockInstance& block) {
    glm::mat3 rotation_scale_mat = glm::mat3(block.modelMatrix);

    // Local OBB 데이터 (StaticModel 내 공유되는 데이터)
    const OBB& road_local_obb = block.modelPtr->road_local_obb;

    //=========================================
    // road_local_obb를 road_world_obb로 변환 (이제 block 인스턴스의 멤버에 씁니다)
    glm::vec4 road_local_center_h = glm::vec4(road_local_obb.center, 1.0f);
    glm::vec3 world_center = glm::vec3(block.modelMatrix * road_local_center_h);

    block.road_world_obb.center = world_center;

    for (int i = 0; i < 3; i++) {
        glm::vec3 world_axis = rotation_scale_mat * road_local_obb.u[i];

        if (glm::length(world_axis) > 1e-6) {
            block.road_world_obb.u[i] = glm::normalize(world_axis);
        }
        else {
            block.road_world_obb.u[i] = road_local_obb.u[i];
        }
    }

    glm::vec3 road_scale_factors = glm::vec3(
        glm::length(rotation_scale_mat[0]),
        glm::length(rotation_scale_mat[1]),
        glm::length(rotation_scale_mat[2])
    );

    block.road_world_obb.half_length = road_local_obb.half_length * road_scale_factors;
    //=========================================

    // obstacle_world_obb 업데이트
    block.obstacle_world_obb.resize(block.modelPtr->obstacle_local_obb.size()); // World OBB 벡터 크기 조정

    for (size_t i = 0; i < block.modelPtr->obstacle_local_obb.size(); i++) {
        const OBB& obstacle_local_obb = block.modelPtr->obstacle_local_obb[i];

        glm::vec4 obstacle_local_center_h = glm::vec4(obstacle_local_obb.center, 1.0f);
        glm::vec3 obs_world_center = glm::vec3(block.modelMatrix * obstacle_local_center_h);

        block.obstacle_world_obb[i].center = obs_world_center;

        for (int j = 0; j < 3; j++) {
            glm::vec3 world_axis = rotation_scale_mat * obstacle_local_obb.u[j];

            if (glm::length(world_axis) > 1e-6) {
                block.obstacle_world_obb[i].u[j] = glm::normalize(world_axis);
            }
            else {
                block.obstacle_world_obb[i].u[j] = obstacle_local_obb.u[j];
            }
        }

        glm::vec3 obstacle_scale_factors = glm::vec3(
            glm::length(rotation_scale_mat[0]),
            glm::length(rotation_scale_mat[1]),
            glm::length(rotation_scale_mat[2])
        );

        block.obstacle_world_obb[i].half_length = obstacle_local_obb.half_length * obstacle_scale_factors;
    }
}

void silver_wolf_update_world_obb(silver_wolf& silverwolf) {
	glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, silverwolf.pos);
    modelMat = glm::rotate(modelMat, glm::radians(silverwolf.angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::scale(modelMat, silverwolf.scale);

	glm::mat3 rotation_scale_mat = modelMat;

	const OBB& silverwolf_local_obb = silverwolf.silverwolf_local_obb;

    glm::vec4 silverwolf_local_center_h = glm::vec4(silverwolf_local_obb.center, 1.0f);
    glm::vec3 world_center = glm::vec3(modelMat * silverwolf_local_center_h);

    silverwolf.silverwolf_world_obb.center = world_center;

    for (int i = 0; i < 3; i++) {
        glm::vec3 world_axis = rotation_scale_mat * silverwolf_local_obb.u[i];

        if (glm::length(world_axis) > 1e-6) {
            silverwolf.silverwolf_world_obb.u[i] = glm::normalize(world_axis);
        }
        else {
            silverwolf.silverwolf_world_obb.u[i] = silverwolf_local_obb.u[i];
        }
    }

    glm::vec3 road_scale_factors = glm::vec3(
        glm::length(rotation_scale_mat[0]),
        glm::length(rotation_scale_mat[1]),
        glm::length(rotation_scale_mat[2])
    );

    silverwolf.silverwolf_world_obb.half_length = silverwolf_local_obb.half_length * road_scale_factors;
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