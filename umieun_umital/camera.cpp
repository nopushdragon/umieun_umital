#include "camera.h"

camera::camera()
{
	
}

camera::~camera()
{
	
}

void camera::Init(glm::vec3 targetPos) {
	camTarget = targetPos;
	camTarget.y += 1.0f;
	camPos = offset + targetPos;
	camera_x_angle = 0.0f;
	camera_y_angle = 0.0f;
	mousex = winWidth / 2;
	mousey = winHeight / 2;

	camUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

void camera::Update(float deltatime, glm::vec3 targetPos) {

	if (right_mouth && offset.z <= -1.5f) {
		offset.z += 0.05f;
		glm::vec3 v = glm::normalize(camPos - camTarget);
		camPos = camTarget + v * (-offset.z);
		camPos.x -= 0.015f;//¹Î¿ë

	}
	else if (!right_mouth && offset.z >= ori_offset.z) {
		offset.z -= 0.05f;
		glm::vec3 v = glm::normalize(camPos - camTarget);
		camPos = camTarget + v * (-offset.z);
		camPos.x += 0.015f;//¹Î¿ë
	}



	targetPos.y += 1.0f;
	camPos += targetPos - camTarget;
	camTarget = targetPos;

}

void camera::Mouse(int button, int state, int x, int y) {
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		right_mouth = true;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		right_mouth = false;
	}
}

void camera::Motion(int x, int y, bool l_alt) {
	camera_move(x, y, l_alt);
}

void camera::PassiveMotion(int x, int y,bool l_alt) {

	camera_move(x, y, l_alt);
}

void camera::camera_move(int x, int y, bool l_alt) {
	if (!l_alt) {
		int center_x = winWidth / 2;
		int center_y = winHeight / 2;

		int deltax = x - mousex;
		int deltay = y - mousey;

		float custom_sense = sense * 2.0f;
		float old_y_angle = camera_y_angle;


		if (deltax != 0) {
			float angle = deltax * custom_sense;

			camera_x_angle += angle;

			if (camera_x_angle < -360.0f) camera_x_angle += 360.0f;
			if (camera_x_angle > 360.0f) camera_x_angle -= 360.0f;

			if (angle != 0.0f) {
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-angle), glm::vec3(0.0f, 1.0f, 0.0f));
				camPos = glm::vec3(rotation * glm::vec4(camPos - camTarget, 1.0f)) + camTarget;
			}
		}

		if (deltay != 0) {
			float angle = -deltay * custom_sense;

			camera_y_angle += angle;

			if (camera_y_angle < -30.0f || camera_y_angle > 30.0f) {
				camera_y_angle = old_y_angle;
				angle = 0.0f;
			}

			if (angle != 0.0f) {
				glm::vec3 c = camPos - camTarget;
				glm::vec3 v = glm::normalize(glm::cross(c, glm::vec3(0.0f, 1.0f, 0.0f)));
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-angle), v);
				camPos = glm::vec3(rotation * glm::vec4(camPos - camTarget, 1.0f)) + camTarget;
			}
		}

		if (x != center_x || y != center_y) {
			mousex = center_x;
			mousey = center_y;
		}
		else {
			center_x = x;
			center_y = y;
		}
	}
}

void camera::Draw(GLuint shaderID) {
	
}

void camera::title_update(float deltatime, glm::vec3 targetPos) {//¹Î¿ë
	if(start_move)
	{
		static float move_t = 0.0f;
		static glm::vec3 start_pos = camPos;
		static glm::vec3 middle_pos = targetPos + glm::vec3(5.0f, 2.0f, 0.0f);
		static glm::vec3 end_pos = targetPos + glm::vec3(-0.2f, 0.65f, 0.4f);

		move_t += 0.01f;

		if (move_t <= 1.0f) {
			float one_minus_t = 1.0f - move_t;

			camPos = (one_minus_t * one_minus_t) * start_pos + (2.0f * one_minus_t * move_t) * middle_pos + (move_t * move_t) * end_pos;

			camTarget = targetPos + glm::vec3(-0.1f, 0.7f, 0.0f);
		}
		else {
			// ÀÌµ¿ ¿Ï·á
			camPos = end_pos;
			start_move = false;
			move_t = 0.0f;
		}

	}
}