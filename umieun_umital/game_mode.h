#pragma once
#include "scene_manager.h"
class game_mode : public Scene
{
public:
	void Enter() override;
	void Update(float deltaTime) override;
	void OnPause() override;
	void OnResume() override;
	void Draw() override;
	void Exit() override;
};