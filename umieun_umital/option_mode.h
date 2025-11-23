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
};

