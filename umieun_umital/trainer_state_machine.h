#pragma once
#include "trainer_state.h"
#include "trainer.h"
class State_Trainer_Walk : public TrainerState
{
public:
	void Enter(Trainer* t) override;
	void Update(Trainer* t, float dt) override;
	void Draw(Trainer* t, GLuint shaderID, float deltatime) override;
	void Exit(Trainer* t) override;
};

class State_Trainer_Tracking : public TrainerState
{
public:
	void Enter(Trainer* t) override;
	void Update(Trainer* t, float dt) override;
	void Draw(Trainer* t, GLuint shaderID, float deltatime) override;
	void Exit(Trainer* t) override;
};

class State_Trainer_Die : public TrainerState
{
public:
	void Enter(Trainer* t) override;
	void Update(Trainer* t, float dt) override;
	void Draw(Trainer* t, GLuint shaderID, float deltatime) override;
	void Exit(Trainer* t) override;
};