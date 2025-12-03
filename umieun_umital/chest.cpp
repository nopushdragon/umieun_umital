#include "chest.h"

void CHEST::init(glm::vec3 reset_matrix) {
	chestInstance new_chest;
	new_chest.modelPtr = chest_model;
	new_chest.reset = reset_matrix + glm::vec3(0.0f,0.0f,-2.5f);	//미로의 1,1칸 위치에서 조금 뒤로 초기 설정
	new_chest.modelMatrix = glm::mat4(1.0f);
	new_chest.modelMatrix = glm::translate(new_chest.modelMatrix, new_chest.reset);
	new_chest.modelMatrix = glm::scale(new_chest.modelMatrix, glm::vec3(0.2f)); //크기 조절
	new_chest.is_nearby = false;
	new_chest.is_in_chunk = false;

	chestBlocks.push_back(new_chest);

	update_world_obb();
}

void CHEST::update() {
}

void CHEST::update_world_obb() {
	for (auto& chest : chestBlocks) {
		glm::mat4 modelMat = chest.modelMatrix;
		glm::mat3 rotation_scale_mat = glm::mat3(modelMat);

		// Local OBB 데이터 (StaticModel 내 공유되는 데이터)
		const OBB& chest_local_obb = chest.modelPtr->chest_local_obb;

		//=========================================
		// chest_local_obb를 chest_world_obb로 변환
		glm::vec4 chest_local_center_h = glm::vec4(chest_local_obb.center, 1.0f);
		glm::vec3 world_center = glm::vec3(modelMat * chest_local_center_h);
		chest.chest_obb.center = world_center;
		chest.chest_nearby_obb.center = world_center;
		for (int i = 0; i < 3; i++) {
			glm::vec3 world_axis = rotation_scale_mat * chest_local_obb.u[i];
			if (glm::length(world_axis) > 1e-6) {
				chest.chest_obb.u[i] = glm::normalize(world_axis);
				chest.chest_nearby_obb.u[i] = glm::normalize(world_axis);
			}
			else {
				chest.chest_obb.u[i] = chest_local_obb.u[i];
				chest.chest_nearby_obb.u[i] = chest_local_obb.u[i];
			}
		}
		glm::vec3 chest_scale_factors = glm::vec3(
			glm::length(rotation_scale_mat[0]),
			glm::length(rotation_scale_mat[1]),
			glm::length(rotation_scale_mat[2])
		);
		chest.chest_obb.half_length = chest_local_obb.half_length * chest_scale_factors;
		chest.chest_nearby_obb.half_length = (chest_local_obb.half_length + 1.0f) * chest_scale_factors;
	}
}

bool CHEST::open_chest(int kill ,int target) {
	chestInstance& chest = chestBlocks[0];
	if (!chest.is_nearby) return false;

	if(target <= kill) return true;
	else return false;
}