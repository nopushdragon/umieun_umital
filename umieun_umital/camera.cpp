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

	}
	else if (!right_mouth && offset.z >= ori_offset.z) {
		offset.z -= 0.05f;
		glm::vec3 v = glm::normalize(camPos - camTarget);
		camPos = camTarget + v * (-offset.z);
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
