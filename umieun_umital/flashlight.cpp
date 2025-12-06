#include "flashlight.h"

void Flashlight::Init() {
	modelPtr = flashlight_model;
	glm::vec3 pos = glm::vec3(0.0f);
	float angle = 180.0f;
	glm::vec3 scale = glm::vec3(0.0002f);
}

void Flashlight::Update(float deltatime, glm::vec3 pos, float angle) {
	this->pos = pos;
	this->pos.y += 0.8f;
	this->angle = -angle+180.0f;


	

	
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, this->pos);
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, scale);
	//cout << modelMatrix[3].x << "," << modelMatrix[3].y << "," << modelMatrix[3].z << endl;
}

void Flashlight::Draw(GLuint shaderID) {
	//cout << "¤·¤µ¤·" << endl;
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	for (auto& mesh : modelPtr->meshes)
	{
		mesh.Draw(shaderID);
		
	}
}
