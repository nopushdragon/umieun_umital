#include "target.h"

void TARGET::init(StaticModel* target_model, int target_cnt) {
	for (int i = 0; i < target_cnt; i++) {
		targetInstance new_target;
		new_target.modelPtr = target_model;
		new_target.modelMatrix = glm::mat4(1.0f);
		new_target.reset = glm::vec3(0.0f);	//초기 시작값은 game_mode에서 설정
		targetBlocks.push_back(new_target);
	}
}

void TARGET::update() {

}

void TARGET::update_world_obb() {
	for(auto& target: targetBlocks) {
		glm::mat4 modelMat = target.modelMatrix;
		glm::mat3 rotation_scale_mat = glm::mat3(modelMat);

		// Local OBB 데이터 (StaticModel 내 공유되는 데이터)
		const OBB& target_local_obb = target.modelPtr->road_local_obb;

		//=========================================
		// target_local_obb를 target_world_obb로 변환
		glm::vec4 target_local_center_h = glm::vec4(target_local_obb.center, 1.0f);
		glm::vec3 world_center = glm::vec3(modelMat * target_local_center_h);
		target.target_obb.center = world_center;
		for (int i = 0; i < 3; i++) {
			glm::vec3 world_axis = rotation_scale_mat * target_local_obb.u[i];
			if (glm::length(world_axis) > 1e-6) {
				target.target_obb.u[i] = glm::normalize(world_axis);
			}
			else {
				target.target_obb.u[i] = target_local_obb.u[i];
			}
		}
		glm::vec3 road_scale_factors = glm::vec3(
			glm::length(rotation_scale_mat[0]),
			glm::length(rotation_scale_mat[1]),
			glm::length(rotation_scale_mat[2])
		);
		target.target_obb.half_length = target_local_obb.half_length * road_scale_factors;
	}
}