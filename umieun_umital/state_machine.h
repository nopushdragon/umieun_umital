#pragma once
#include "silver_wolf_state.h"
#include "silver_wolf.h"

class State_Idle : public WolfState {
public:
	void Enter(silver_wolf* wolf)override;
	void Update(silver_wolf* wolf, float detatime)override;
	void Draw(silver_wolf* wolf, GLuint shaderID, float time)override;
	void Exit(silver_wolf* wolf)override;

};


class State_Walk : public WolfState {
public:
	void Enter(silver_wolf* wolf)override;
	void Update(silver_wolf* wolf, float detatime)override;
	void Draw(silver_wolf* wolf, GLuint shaderID, float time)override;
	void Exit(silver_wolf* wolf)override;

};



class State_Run : public WolfState {
public:
	void Enter(silver_wolf* wolf)override;
	void Update(silver_wolf* wolf, float detatime)override;
	void Draw(silver_wolf* wolf, GLuint shaderID, float time)override;
	void Exit(silver_wolf* wolf)override;

};


class State_Throw : public WolfState {
public:
	void Enter(silver_wolf* wolf)override;
	void Update(silver_wolf* wolf, float detatime)override;
	void Draw(silver_wolf* wolf, GLuint shaderID, float time)override;
	void Exit(silver_wolf* wolf)override;

};

class State_Roll : public WolfState {
public:
	void Enter(silver_wolf* wolf)override;
	void Update(silver_wolf* wolf, float detatime)override;
	void Draw(silver_wolf* wolf, GLuint shaderID, float time)override;
	void Exit(silver_wolf* wolf)override;

};

class State_Jump : public WolfState {
public:
	void Enter(silver_wolf* wolf)override;
	void Update(silver_wolf* wolf, float detatime)override;
	void Draw(silver_wolf* wolf, GLuint shaderID, float time)override;
	void Exit(silver_wolf* wolf)override;

};

class State_Jump_Run : public WolfState {
public:
	void Enter(silver_wolf* wolf)override;
	void Update(silver_wolf* wolf, float detatime)override;
	void Draw(silver_wolf* wolf, GLuint shaderID, float time)override;
	void Exit(silver_wolf* wolf)override;

};
class State_Stop_Run : public WolfState {
public:
	void Enter(silver_wolf* wolf)override;
	void Update(silver_wolf* wolf, float detatime)override;
	void Draw(silver_wolf* wolf, GLuint shaderID, float time)override;
	void Exit(silver_wolf* wolf)override;

};
class State_Jump_Idle : public WolfState {
public:
	void Enter(silver_wolf* wolf)override;
	void Update(silver_wolf* wolf, float detatime)override;
	void Draw(silver_wolf* wolf, GLuint shaderID, float time)override;
	void Exit(silver_wolf* wolf)override;

};



