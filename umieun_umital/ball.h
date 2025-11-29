#pragma once
#include "headers.h"
#include "static_model.h"

class Ball
{
private:
	StaticModel* modelPtr;
	glm::mat4 modelMatrix;

	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;

	glm::vec3 current_pos;

	bool end_pos = false;


	//미로에 쓸거면 트루로 ㄱㄱ
	bool maze = false;

	
	float speed = 2.0f;

public:
	OBB ball_obb;

	int t = 0;

	Ball(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2,bool maze);

	void Init();

	void Update(float deltatime);

	void Draw(GLuint shaderID);

	void update_world_obb();

};

