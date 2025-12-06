#pragma once
#include "resource.h"	


class Flashlight
{
private:
	StaticModel* modelPtr;
	glm::mat4 modelMatrix;
	glm::vec3 scale= glm::vec3(0.0002f);
public:
	glm::vec3 pos=glm::vec3(0.0f);
	float angle=180.0f;
	void Init();

	void Update(float deltatime,glm::vec3 pos,float angle);

	void Draw(GLuint shaderID);
};

