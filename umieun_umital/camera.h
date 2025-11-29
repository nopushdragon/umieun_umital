#pragma once
#include "static_model.h"
class camera
{
private:
	int mousex = winWidth / 2;
	int mousey = winHeight / 2;
	//카메라 감도
	float sense = 0.01f;
	//카메라 거리
	glm::vec3 offset = glm::vec3(0.0f, 1.0f, -2.0f);
	glm::vec3 ori_offset = glm::vec3(0.0f, 1.0f, -2.0f);
public:
	//카메라 각도
	float camera_x_angle = 0.0f;
	float camera_y_angle = 0.0f;
	glm::vec3 camPos;
	glm::vec3 camTarget;
	glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
	OBB camera_local_obb;
	OBB camera_world_obb;
	//마우스 클릭
	bool right_mouth = false;
	camera();
	~camera();
	void Init(glm::vec3 targetPos);
	void Update(float deltatime, glm::vec3 targetPos);
	void PassiveMotion(int x, int y,bool l_alt);
	void Draw(GLuint shaderID);
	void Mouse(int button, int state, int x, int y);
	void Motion(int x, int y, bool l_alt);

	void camera_move(int x, int y, bool l_alt);

	void title_update(float deltatime, glm::vec3 targetPos);//민용
	bool start_move = false;
};

