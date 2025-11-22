#pragma once
#include "animated_model.h"
class silver_wolf
{
public:

	NewModel* silverWolfModel[7];
	glm::mat4 modelMat = glm::mat4(1.0f);
	int silver_wolf_fbx_size = 6;
	string state = "idle";
public:
	silver_wolf();

	void Update(float currentTime);

	void Draw(GLuint shaderID, float currentTime);

	~silver_wolf();



};

