#include "target.h"

void TARGET::init(StaticModel* target_model, int target_cnt) {
	for (int i = 0; i < target_cnt; i++) {
		targetInstance new_target;
		new_target.modelPtr = target_model;
		new_target.modelMatrix = glm::mat4(1.0f);
		new_target.reset = glm::vec3(0.0f);	//초기 시작값은 game_mode에서 설정
		new_target.is_break = false;
		new_target.is_in_chunk = false;

		uniform_real_distribution<float> rd_rotate(0.0f, 360.0f);
		new_target.rotation_angle = rd_rotate(mt);


		targetBlocks.push_back(new_target);
	}
}

void TARGET::update() {
	const float AMPLITUDE = 0.5f; // 최대 0.5 상승/하강
	const float SPEED_FACTOR = 0.05f; // 움직임 속도 조절

	for (auto& target : targetBlocks) {
		if (target.is_break || !target.is_in_chunk) continue;

		// 목적지 설정 및 이동
		if (target.move_t >= 1.5f) {
			target.move_t = 0.0f;
		}

		if (target.move_t == 0.0f) {
			uniform_real_distribution<float> rd_goal_x(target.reset.x - 5.0f, target.reset.x + 5.0f);
			uniform_real_distribution<float> rd_goal_y(target.reset.y - 1.2f, target.reset.y + 1.2f);
			uniform_real_distribution<float> rd_goal_z(target.reset.z - 5.0f, target.reset.z + 5.0f);

			target.origin_pos = target.modelMatrix[3];
			target.goal_pos = glm::vec3(rd_goal_x(mt), rd_goal_y(mt), rd_goal_z(mt));
		}
		target.move_t += 0.002f;
		glm::vec3 move_pos;
		if (target.move_t <= 1.0f) move_pos = (1.0f - target.move_t) * target.origin_pos + target.move_t * target.goal_pos;
		else move_pos = target.goal_pos;

		// 목적지 바라보기
		glm::vec3 direction = target.goal_pos - move_pos;
		direction.y = 0.0f;
		if (glm::length(direction) > 0.0001f) {
			float angle_rad = atan2(direction.x, direction.z);
			angle_rad -= glm::radians(90.0f);
			target.rotation_angle = glm::degrees(angle_rad);
		}

		// 둥실둥실
		target.timer++;
		float float_y_offset = AMPLITUDE * std::sin((float)target.timer * SPEED_FACTOR);
		move_pos.y += float_y_offset;

		target.modelMatrix = glm::mat4(1.0f);
		target.modelMatrix = glm::translate(target.modelMatrix, move_pos);
		target.modelMatrix = glm::rotate(target.modelMatrix, glm::radians(target.rotation_angle), glm::vec3(0.0f, 1.0f, 0.0f));
		target.modelMatrix = glm::scale(target.modelMatrix, glm::vec3(0.3f));
	}

	update_world_obb();
}

void TARGET::update_world_obb() {
	for(auto& target: targetBlocks) {
		if (target.is_break || !target.is_in_chunk) continue;
		glm::mat4 modelMat = target.modelMatrix;
		glm::mat3 rotation_scale_mat = glm::mat3(modelMat);

		// Local OBB 데이터 (StaticModel 내 공유되는 데이터)
		const OBB& target_local_obb = target.modelPtr->target_local_obb;

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
		glm::vec3 target_scale_factors = glm::vec3(
			glm::length(rotation_scale_mat[0]),
			glm::length(rotation_scale_mat[1]),
			glm::length(rotation_scale_mat[2])
		);
		target.target_obb.half_length = target_local_obb.half_length * target_scale_factors;
	}
}