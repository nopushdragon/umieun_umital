#pragma once
#include "scene_manager.h"
#include "image.h"
#include "text.h"
#include "camera.h"
class logo_mode : public Scene
{
private:
	Image* background_Image = nullptr;
	Image* tuk_image = nullptr;
	Image* logo_Image = nullptr;

	camera camera;
	int WINDOW_WIDTH = winWidth;
	int WINDOW_HEIGHT = winHeight;

	// 조명 및 재질 설정
	glm::vec3 lightPos;
	glm::vec3 lightColor;
	glm::vec3 materialSpecular;
	float ambientStrength;
	int shininess;


public:
	GLuint uiShaderProgram;





	void Init() override;
	void Update(float deltaTime) override;
	void OnPause() override;
	void OnResume() override;
	void Draw() override;
	void Finish() override;
	void Reshape(int w, int h) override;

	void Mouse(int button, int state, int x, int y) override;
	//void MouseWheel(int wheel, int direction, int x, int y) override;
	//void Motion(int x, int y) override;
	//void PassiveMotion(int x, int y) override;
	void Keyboard(unsigned char key, int x, int y) override;
	//void Keyupboard(unsigned char key, int x, int y) override;

	//void SpecialKeyboard(int key, int x, int y) override;
	//void SpecialUpKeyboard(int key, int x, int y) override;

};

