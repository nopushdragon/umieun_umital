#pragma once
#include "headers.h"
#include "static_model.h"
#include "resource.h"
#include "collision.h"

class Ball
{
private:
	StaticModel* modelPtr;
	glm::mat4 modelMatrix;

	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;




	//미로에 쓸거면 트루로 ㄱㄱ
	bool maze = false;

	
	float speed = 200.0f;

	float angle = 0.0f;

public:
	glm::vec3 current_pos;
	//p2에 도착했을경우
	bool end_pos = false;
	
	OBB ball_obb;
	bool is_in_chunk = false;
	bool is_nearby = false;

	float t = 0;

	Ball(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2,bool maze);

	void Init();


	void Update(float deltatime);

	void Draw(GLuint shaderID);

	void update_world_obb();


	void DebugOBB(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color);

};

