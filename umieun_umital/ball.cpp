#include "ball.h"



Ball::Ball(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2,bool maze) {
	this->p0 = p0;
	this->current_pos = p0;
	this->p1 = p1;
	this->p2 = p2;

	this->maze = maze;

}

void Ball::Init() {
	modelPtr = new StaticModel("ball/Pokeball.obj");
	t = 0;
	
}

void Ball::Update(float deltatime) {
	
	if (!maze) {
		t += speed * deltatime;
		float t_normalized = t / 500.0f;
		this->current_pos = (1 - t_normalized) * (1 - t_normalized) * p0 + 2 * (1 - t_normalized) * t_normalized * p1 + t_normalized * t_normalized * p2;
		if (t >= 500) {
			end_pos = true;
		}
	}


	update_world_obb();
}


void Ball::Draw(GLuint shaderID) {
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	modelPtr->meshes.back().Draw(shaderID);
}

void Ball::update_world_obb() {
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, current_pos);
	modelMat = glm::scale(modelMat, glm::vec3(0.5f));
	glm::mat3 rotation_scale_mat = glm::mat3(modelMat);

	
	const OBB& ball_local_obb = modelPtr->chest_local_obb;

	
	glm::vec4 ball_local_center_h = glm::vec4(ball_local_obb.center, 1.0f);
	glm::vec3 world_center = glm::vec3(modelMat * ball_local_center_h);
	ball_obb.center = world_center;
	for (int i = 0; i < 3; i++) {
		glm::vec3 world_axis = rotation_scale_mat * ball_local_obb.u[i];
		if (glm::length(world_axis) > 1e-6) {
			ball_obb.u[i] = glm::normalize(world_axis);
		}
		else {
			ball_obb.u[i] = ball_local_obb.u[i];
		}
	}
	glm::vec3 chest_scale_factors = glm::vec3(
		glm::length(rotation_scale_mat[0]),
		glm::length(rotation_scale_mat[1]),
		glm::length(rotation_scale_mat[2])
	);
	ball_obb.half_length = ball_local_obb.half_length * chest_scale_factors;
}

