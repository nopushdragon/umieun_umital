#pragma once
#include "scene_manager.h"
class option_mode : public Scene
{
public:
	void Init() override;
	void Update(float deltaTime) override;
	void OnPause() override;
	void OnResume() override;
	void Draw() override;
	void Finish() override;
	void Reshape(int w, int h) override;

	void Mouse(int button, int state, int x, int y) override;
	void MouseWheel(int wheel, int direction, int x, int y) override;
	void Motion(int x, int y) override;
	void PassiveMotion(int x, int y) override;
	void Keyboard(unsigned char key, int x, int y) override;
	void Keyupboard(unsigned char key, int x, int y) override;

	void SpecialKeyboard(int key, int x, int y) override;
	void SpecialUpKeyboard(int key, int x, int y) override;
};

