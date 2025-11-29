#include "ball.h"



Ball::Ball(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2,bool maze) {
	this->p0 = p0;
	this->current_pos = p0;
	this->p1 = p1;
	this->p2 = p2;

	this->maze = maze;



}

void Ball::Init() {
	modelPtr = ball_model;

	
}

void Ball::Update(float deltatime) {
	
	if (!maze) {
		t += speed * deltatime;
		float t_normalized = t / 500.0f;
		this->current_pos = (1 - t_normalized) * (1 - t_normalized) * p0 + 2 * (1 - t_normalized) * t_normalized * p1 + t_normalized * t_normalized * p2;
		if (t >= 500) {
			end_pos = true;
		}
		angle += 1.0f;

		//cout << "ball p0: " <<p0.x << ", " << p0.y << ", " << p0.z << endl;
		//cout << "ball p1: " << p1.x << ", " << p1.y << ", " << p1.z << endl;
		//cout << "ball p2: " << p2.x << ", " << p2.y << ", " << p2.z << endl;
		//cout << "ball pos: " << current_pos.x << ", " << current_pos.y << ", " << current_pos.z << endl;
	}


	update_world_obb();
}


void Ball::Draw(GLuint shaderID) {
	

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	for (auto& mesh : modelPtr->meshes)
	{
		mesh.Draw(shaderID);
	}
}

void Ball::update_world_obb() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, current_pos);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f + angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0005f));

	glm::mat3 rotation_scale_mat = modelMatrix;

	
	const OBB& ball_local_obb = modelPtr->ball_local_obb;

	
	glm::vec4 ball_local_center_h = glm::vec4(ball_local_obb.center, 1.0f);
	glm::vec3 world_center = glm::vec3(modelMatrix * ball_local_center_h);
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
	glm::vec3 ball_scale_factors = glm::vec3(
		glm::length(rotation_scale_mat[0]),
		glm::length(rotation_scale_mat[1]),
		glm::length(rotation_scale_mat[2])
	);
	ball_obb.half_length = ball_local_obb.half_length * ball_scale_factors;
}

void Ball::DebugOBB(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color) {
	drawDebugOBB(shaderID, ball_obb, view, proj, color);
}

